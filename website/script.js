// The UI sends controls and transport changes. Audio and step timing live in
// trx-worklet.js; synthesis lives in the shared C sources compiled to WASM.
const machines = {
    0: {name: 'TRX-B2', labels: ['PTCH', 'DEC', 'RAMP', 'HOLD', 'TICK', 'NOIS', 'DIRT', 'DIST']},
    1: {name: 'TRX-SD', labels: ['PTCH', 'DEC', 'BUMP', 'BENV', 'SNAP', 'TONE', 'TUNE', 'CLIP']},
    2: {name: 'TRX-CH', labels: ['GAP', 'DEC', 'HPF', 'LPF', 'MTAL']},
    3: {name: 'TRX-OH', labels: ['GAP', 'DEC', 'HPF', 'LPF', 'MTAL']},
    4: {name: 'TRX-CY', labels: ['RICH', 'DEC', 'TOP', 'TTUN', 'SIZE', 'PEAK']},
    5: {name: 'TRX-RS', labels: ['PTCH', 'DEC', 'DIST']},
    6: {name: 'TRX-CB', labels: ['PTCH', 'DEC', 'ENH', 'TONE', 'BUMP', '—', '—', 'DIST']},
    7: {name: 'TRX-CL', labels: ['PTCH', 'DEC', 'DUAL', 'ENH', 'TUNE', 'CLIC']},
    8: {name: 'EFM-BD', labels: ['PTCH', 'DEC', 'RAMP', 'RDEC', 'MOD', 'MFRQ', 'MDEC', 'MFB']},
    9: {name: 'EFM-SD', labels: ['PTCH', 'DEC', 'NOISE', 'NDEC', 'MOD', 'MFRQ', 'MDEC', 'HPF']},
    10: {name: 'EFM-XT', labels: ['PTCH', 'DEC', 'RAMP', 'RDEC', 'MOD', 'MFRQ', 'MDEC', 'CLIC']},
    11: {name: 'EFM-CP', labels: ['PTCH', 'DEC', 'CLPS', 'CDEC', 'MOD', 'MFRQ', 'MDEC', 'HPF']},
    12: {name: 'EFM-RS', labels: ['PTCH', 'DEC', 'MOD', 'HPF', 'SNAR', 'SPTC', 'SDEC', 'SMOD']},
    13: {name: 'EFM-CB', labels: ['PTCH', 'DEC', 'SNAP', 'FB', 'MOD', 'MFRQ', 'MDEC']},
    14: {name: 'EFM-HH', labels: ['PTCH', 'DEC', 'TREM', 'TFRQ', 'MOD', 'MFRQ', 'MDEC', 'FB']},
    15: {name: 'EFM-CY', labels: ['PTCH', 'DEC', 'FB', 'HPF', 'MOD', 'MFRQ', 'MDEC']}
};

class MachineAudio {
    constructor(onStep, onStatus, onDefaults, onTweaks) {
        this.onStep = onStep;
        this.onStatus = onStatus;
        this.onDefaults = onDefaults;
        this.onTweaks = onTweaks;
        this.kind = 0;
        this.controls = Array(8).fill(0);
        this.defaults = null;
        this.tweakDescriptors = [];
        this.tweakValuesByKind = new Map();
        this.level = 127;
        this.steps = Array(16).fill(false);
        this.tempo = 120;
        this.context = null;
        this.node = null;
        this.ready = null;
    }

    async ensureReady() {
        if (!this.ready) this.ready = this.initialize().catch(error => {
            this.ready = null;
            this.onStatus(error.message, true);
            throw error;
        });
        return this.ready;
    }

    async initialize() {
        if (!window.AudioWorkletNode) throw new Error('This browser needs AudioWorklet support.');
        if (!this.context || this.context.state === 'closed') {
            this.context = new AudioContext({sampleRate: 48000});
        }
        if (this.context.sampleRate !== 48000) {
            throw new Error(`The C voice requires 48 kHz; this browser opened ${this.context.sampleRate} Hz.`);
        }
        await this.context.audioWorklet.addModule('trx-worklet.js');
        const response = await fetch('trx-synth.wasm');
        if (!response.ok) throw new Error(`Could not load trx-synth.wasm (${response.status}).`);
        const bytes = await response.arrayBuffer();
        this.node = new AudioWorkletNode(this.context, 'trx-processor', {
            numberOfInputs: 0,
            numberOfOutputs: 1,
            outputChannelCount: [2]
        });
        this.node.connect(this.context.destination);
        const wasmReady = new Promise((resolve, reject) => {
            this.node.port.onmessage = event => {
                const message = event.data;
                if (message.type === 'ready') {
                    this.tweakDescriptors = message.tweakDescriptors;
                    this.tweakValuesByKind.set(message.kind, message.tweakValues);
                    resolve(message.defaults);
                }
                else if (message.type === 'error') reject(new Error(message.message));
                else if (message.type === 'step') this.onStep(message.index);
                else if (message.type === 'tweak-values') {
                    if (message.kind === this.kind) this.tweakDescriptors = message.descriptors;
                    this.tweakValuesByKind.set(message.kind, message.values);
                    if (message.kind === this.kind) this.onTweaks(message.kind, message.values);
                } else if (message.type === 'tweak-value') {
                    const values = this.tweakValuesByKind.get(message.kind);
                    if (values) {
                        values[message.index] = message.value;
                        if (message.kind === this.kind) this.onTweaks(message.kind, values);
                    }
                }
            };
        });
        this.node.port.postMessage({type: 'wasm', bytes}, [bytes]);
        this.defaults = await wasmReady;
        this.onDefaults();
        this.send({type: 'level', value: this.level});
        this.send({type: 'tempo', value: this.tempo});
        this.steps.forEach((active, index) => this.send({type: 'step-active', index, active}));
        this.onStatus('Audio ready: shared C Digidrum voice at 48 kHz.');
    }

    send(message) {
        if (this.node) this.node.port.postMessage(message);
    }

    setKind(kind) {
        this.kind = kind;
        this.controls = this.defaults ? [...this.defaults[kind].controls] : Array(8).fill(0);
        this.send({type: 'kind', kind, tweaks: this.tweakValuesByKind.get(kind)});
        this.controls.forEach((value, index) => this.send({type: 'control', index, value}));
    }

    setTweak(index, value) {
        const values = this.tweakValuesByKind.get(this.kind);
        if (!values || !this.tweakDescriptors[index]) return;
        const desc = this.tweakDescriptors[index];
        const next = Math.max(desc.min, Math.min(desc.max, Math.round(value)));
        values[index] = next;
        this.send({type: 'tweak-set', index, value: next});
    }

    setControl(index, value) {
        this.controls[index] = value;
        this.send({type: 'control', index, value});
    }

    setLevel(value) {
        this.level = value;
        this.send({type: 'level', value});
    }

    setTempo(value) {
        this.tempo = value;
        this.send({type: 'tempo', value});
    }

    setStep(index, active) {
        this.steps[index] = active;
        this.send({type: 'step-active', index, active});
    }

    async trigger() {
        await this.ensureReady();
        await this.context.resume();
        this.send({type: 'trigger'});
    }

    async play() {
        await this.ensureReady();
        await this.context.resume();
        this.send({type: 'play'});
    }

    stop() {
        this.send({type: 'stop'});
    }
}

document.addEventListener('DOMContentLoaded', () => {
    const status = document.getElementById('audio-status');
    const steps = [...document.querySelectorAll('.step')];
    const playhead = document.querySelector('.playhead');
    const wrappers = [...document.querySelectorAll('.knob-wrapper')];
    const machineSelect = document.getElementById('machine-select');
    const level = document.getElementById('level');
    const tempo = document.getElementById('tempo');
    const algorithmDetails = document.getElementById('algorithm-editor');
    const algorithmStatus = document.getElementById('algorithm-status');
    let algorithmEditor = null;
    let editorModule = null;

    async function showAlgorithm(kind, values) {
        if (kind !== audio.kind || !algorithmDetails.open || !values) return;
        const needsEditor = !algorithmEditor || algorithmEditor.kind !== kind;
        if (needsEditor) algorithmStatus.textContent = 'Loading algorithm controls…';
        try {
            editorModule ??= import('./algorithm-editor.js');
            const {AlgorithmEditor} = await editorModule;
            if (kind !== audio.kind || !algorithmDetails.open) return;
            algorithmEditor ??= new AlgorithmEditor(
                document.getElementById('algorithm-pane'),
                (index, value) => audio.setTweak(index, value)
            );
            if (algorithmEditor.kind === kind)
                algorithmEditor.update(values);
            else
                algorithmEditor.show(kind, audio.tweakDescriptors, values);
            if (needsEditor)
                algorithmStatus.textContent = `${machines[kind].name} algorithm settings are live.`;
        } catch (error) {
            editorModule = null;
            algorithmStatus.textContent = `Could not load Tweakpane: ${error.message}`;
        }
    }

    const audio = new MachineAudio(index => {
        steps.forEach((step, i) => step.classList.toggle('current', i === index));
        playhead.style.left = `${index * 28}px`;
    }, (message, error = false) => {
        status.textContent = message;
        status.classList.toggle('error', error);
    }, () => machineSelect.dispatchEvent(new Event('change')),
    (kind, values) => showAlgorithm(kind, values));

    function showKnob(wrapper, value) {
        wrapper.dataset.value = String(value);
        wrapper.querySelector('.param-value').textContent = String(value);
        wrapper.querySelector('.ring-fill').style.width = `${value * 100 / 127}%`;
        wrapper.setAttribute('aria-valuenow', String(value));
    }

    wrappers.forEach((wrapper, index) => {
        const value = audio.controls[index];
        wrapper.dataset.letter = String.fromCharCode(65 + index);
        const readout = document.createElement('span');
        readout.className = 'param-value';
        wrapper.append(readout);
        wrapper.setAttribute('role', 'slider');
        wrapper.setAttribute('tabindex', '0');
        wrapper.setAttribute('aria-valuemin', '0');
        wrapper.setAttribute('aria-valuemax', '127');
        showKnob(wrapper, value);

        const knob = wrapper.querySelector('.knob-container');
        knob.addEventListener('pointerdown', event => {
            event.preventDefault();
            knob.setPointerCapture(event.pointerId);
            const move = moved => {
                const bounds = knob.getBoundingClientRect();
                const position = Math.max(0, Math.min(1, (moved.clientX - bounds.left) / bounds.width));
                const next = Math.round(position * 127);
                showKnob(wrapper, next);
                audio.setControl(index, next);
            };
            move(event);
            knob.addEventListener('pointermove', move);
            knob.addEventListener('pointerup', () => knob.removeEventListener('pointermove', move), {once: true});
            knob.addEventListener('pointercancel', () => knob.removeEventListener('pointermove', move), {once: true});
        });
        wrapper.addEventListener('keydown', event => {
            const direction = event.key === 'ArrowUp' || event.key === 'ArrowRight' ? 1 :
                event.key === 'ArrowDown' || event.key === 'ArrowLeft' ? -1 : 0;
            if (!direction) return;
            event.preventDefault();
            const next = Math.max(0, Math.min(127, Number(wrapper.dataset.value) + direction));
            showKnob(wrapper, next);
            audio.setControl(index, next);
        });
    });

    machineSelect.addEventListener('change', () => {
        const kind = Number(machineSelect.value);
        audio.setKind(kind);
        wrappers.forEach((wrapper, index) => {
            const name = machines[kind].labels[index];
            wrapper.hidden = !name;
            if (name) {
                wrapper.querySelector('.param-name').textContent = name;
                wrapper.setAttribute('aria-label', name);
                showKnob(wrapper, audio.controls[index]);
            }
        });
    });
    machineSelect.dispatchEvent(new Event('change'));
    audio.ensureReady().catch(() => {}); // Load C defaults while audio remains suspended.

    algorithmDetails.addEventListener('toggle', () => {
        if (algorithmDetails.open) showAlgorithm(audio.kind, audio.tweakValuesByKind.get(audio.kind));
    });

    level.addEventListener('input', () => {
        const value = Number(level.value);
        document.getElementById('level-value').textContent = String(value);
        audio.setLevel(value);
    });
    tempo.addEventListener('input', () => {
        const value = Number(tempo.value);
        document.getElementById('tempo-value').textContent = String(value);
        audio.setTempo(value);
    });
    steps.forEach((step, index) => step.addEventListener('click', () => {
        const active = step.classList.toggle('active');
        audio.setStep(index, active);
    }));
    document.getElementById('trigger-btn').addEventListener('click', () => {
        audio.trigger().catch(() => {});
    });
    document.getElementById('play-btn').addEventListener('click', () => {
        audio.play().catch(() => {});
    });
    document.getElementById('stop-btn').addEventListener('click', () => {
        audio.stop();
        steps.forEach(step => step.classList.remove('current'));
    });

});
