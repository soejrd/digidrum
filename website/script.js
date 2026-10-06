// The UI sends controls and transport changes. Audio and step timing live in
// trx-worklet.js; synthesis lives in the shared C sources compiled to WASM.
const machines = {
    0: {name: 'TRX-BD', labels: ['PTCH', 'DEC', 'RAMP', 'RDEC', 'STRT', 'NOIS', 'HARM', 'CLIP']},
    1: {name: 'TRX-B2', labels: ['PTCH', 'DEC', 'RAMP', 'HOLD', 'TICK', 'NOIS', 'DIRT', 'DIST']},
    2: {name: 'TRX-SD', labels: ['PTCH', 'DEC', 'BUMP', 'BENV', 'SNAP', 'TONE', 'TUNE', 'CLIP']}
};

class MachineAudio {
    constructor(onStep, onStatus) {
        this.onStep = onStep;
        this.onStatus = onStatus;
        this.kind = 1;
        this.controls = [64, 64, 64, 0, 64, 0, 0, 0];
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
        await this.context.resume();
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
                if (message.type === 'ready') resolve();
                else if (message.type === 'error') reject(new Error(message.message));
                else if (message.type === 'step') this.onStep(message.index);
            };
        });
        this.node.port.postMessage({type: 'wasm', bytes}, [bytes]);
        await wasmReady;
        this.send({type: 'kind', kind: this.kind});
        this.controls.forEach((value, index) => this.send({type: 'control', index, value}));
        this.send({type: 'level', value: this.level});
        this.send({type: 'tempo', value: this.tempo});
        this.steps.forEach((active, index) => this.send({type: 'step-active', index, active}));
        this.onStatus('Audio ready: shared C TRX voice at 48 kHz.');
    }

    send(message) {
        if (this.node) this.node.port.postMessage(message);
    }

    setKind(kind) {
        this.kind = kind;
        this.send({type: 'kind', kind});
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
    const audio = new MachineAudio(index => {
        steps.forEach((step, i) => step.classList.toggle('current', i === index));
        playhead.style.left = `${index * 28}px`;
    }, (message, error = false) => {
        status.textContent = message;
        status.classList.toggle('error', error);
    });

    function showKnob(wrapper, value) {
        wrapper.dataset.value = String(value);
        wrapper.querySelector('.param-value').textContent = String(value);
        wrapper.querySelector('.ring-fill').style.width = `${value * 100 / 127}%`;
        wrapper.setAttribute('aria-valuenow', String(value));
    }

    wrappers.forEach((wrapper, index) => {
        const value = Number(wrapper.dataset.value);
        wrapper.dataset.letter = String.fromCharCode(65 + index);
        const readout = document.createElement('span');
        readout.className = 'param-value';
        wrapper.append(readout);
        wrapper.setAttribute('role', 'slider');
        wrapper.setAttribute('tabindex', '0');
        wrapper.setAttribute('aria-valuemin', '0');
        wrapper.setAttribute('aria-valuemax', '127');
        showKnob(wrapper, value);
        audio.setControl(index, value);

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
            wrapper.querySelector('.param-name').textContent = name;
            wrapper.setAttribute('aria-label', name);
        });
    });
    machineSelect.dispatchEvent(new Event('change'));

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
