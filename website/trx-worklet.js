/* global AudioWorkletProcessor, registerProcessor, sampleRate */
class TrxProcessor extends AudioWorkletProcessor {
    constructor() {
        super();
        this.wasm = null;
        this.samples = null;
        this.capacity = 0;
        this.kind = 0;
        this.controls = Array(8).fill(0);
        this.level = 127;
        this.tempo = 120;
        this.steps = Array(16).fill(false);
        this.nextStep = 0;
        this.samplesUntilStep = 0;
        this.playing = false;
        this.pendingHit = false;
        this.port.onmessage = event => this.onMessage(event.data);
    }

    readCString(pointer) {
        const bytes = new Uint8Array(this.wasm.memory.buffer);
        let name = '';
        while (pointer && bytes[pointer]) name += String.fromCharCode(bytes[pointer++]);
        return name;
    }

    tweakValues() {
        return Array.from({length: this.wasm.dd_web_efm_tweak_count()}, (_, index) =>
            this.wasm.dd_web_efm_tweak_get(index));
    }

    async onMessage(message) {
        try {
            if (message.type === 'wasm') {
                if (sampleRate !== 48000) throw new Error(`Digidrum DSP expects 48 kHz; got ${sampleRate} Hz.`);
                const loaded = await WebAssembly.instantiate(message.bytes);
                this.wasm = loaded.instance.exports;
                this.capacity = this.wasm.dd_web_capacity();
                const defaults = Array.from({length: this.wasm.dd_web_machine_count()}, (_, kind) => ({
                    controls: Array.from({length: 8}, (_, index) => this.wasm.dd_web_default_control(kind, index)),
                    count: this.wasm.dd_web_control_count(kind)
                }));
                const tweakDescriptors = Array.from({length: this.wasm.dd_web_efm_tweak_count()}, (_, index) => ({
                    name: this.readCString(this.wasm.dd_web_efm_tweak_name(index)),
                    min: this.wasm.dd_web_efm_tweak_min(index),
                    max: this.wasm.dd_web_efm_tweak_max(index)
                }));
                this.controls = [...defaults[this.kind].controls];
                this.wasm.dd_web_init(this.kind);
                this.controls.forEach((value, index) => this.wasm.dd_web_set_control(index, value));
                this.wasm.dd_web_set_level(this.level);
                const pointer = this.wasm.dd_web_render(0);
                this.samples = new Int32Array(this.wasm.memory.buffer, pointer, this.capacity);
                this.port.postMessage({type: 'ready', kind: this.kind, defaults, tweakDescriptors,
                    tweakValues: this.tweakValues()});
            } else if (message.type === 'kind') {
                this.kind = message.kind;
                if (this.wasm) {
                    this.wasm.dd_web_init(this.kind);
                    this.controls.forEach((value, index) => this.wasm.dd_web_set_control(index, value));
                    this.wasm.dd_web_set_level(this.level);
                    if (this.kind >= 8 && Array.isArray(message.tweaks))
                        message.tweaks.forEach((value, index) =>
                            this.wasm.dd_web_efm_tweak_set(index, value));
                    this.port.postMessage({type: 'tweak-values', kind: this.kind,
                        values: this.tweakValues()});
                }
            } else if (message.type === 'tweak-get' && this.wasm) {
                this.port.postMessage({type: 'tweak-value', kind: this.kind,
                    index: message.index,
                    value: this.wasm.dd_web_efm_tweak_get(message.index)});
            } else if (message.type === 'tweak-set' && this.wasm) {
                this.wasm.dd_web_efm_tweak_set(message.index, message.value);
                this.port.postMessage({type: 'tweak-value', kind: this.kind,
                    index: message.index,
                    value: this.wasm.dd_web_efm_tweak_get(message.index)});
            } else if (message.type === 'control') {
                if (message.index >= 0 && message.index < 8) {
                    this.controls[message.index] = message.value;
                    if (this.wasm) this.wasm.dd_web_set_control(message.index, message.value);
                }
            } else if (message.type === 'level') {
                this.level = message.value;
                if (this.wasm) this.wasm.dd_web_set_level(this.level);
            } else if (message.type === 'tempo') {
                this.tempo = Math.max(30, Math.min(240, Number(message.value) || 120));
            } else if (message.type === 'step-active') {
                if (message.index >= 0 && message.index < 16)
                    this.steps[message.index] = Boolean(message.active);
            } else if (message.type === 'trigger') {
                this.pendingHit = true;
            } else if (message.type === 'play') {
                this.nextStep = 0;
                this.samplesUntilStep = 0;
                this.playing = true;
            } else if (message.type === 'stop') {
                this.playing = false;
            }
        } catch (error) {
            this.port.postMessage({type: 'error', message: String(error.message || error)});
        }
    }

    process(_inputs, outputs) {
        const stereo = outputs[0];
        if (!stereo || !stereo[0]) return true;
        const left = stereo[0];
        const right = stereo[1];
        if (!this.wasm) return true; // Web Audio initializes output to silence.

        let offset = 0;
        while (offset < left.length) {
            if (this.playing && this.samplesUntilStep <= 0) {
                const index = this.nextStep;
                if (this.steps[index]) this.pendingHit = true;
                this.port.postMessage({type: 'step', index});
                this.nextStep = (index + 1) % 16;
                this.samplesUntilStep += sampleRate * 60 / (this.tempo * 4);
            }
            if (this.pendingHit) {
                this.wasm.dd_web_trigger();
                this.pendingHit = false;
            }
            const untilStep = this.playing ? Math.max(1, Math.ceil(this.samplesUntilStep)) : left.length;
            const frames = Math.min(this.capacity, left.length - offset, untilStep);
            this.wasm.dd_web_render(frames);
            for (let i = 0; i < frames; i++) {
                const value = Math.max(-1, Math.min(1, this.samples[i] / 2147483648));
                left[offset + i] = value;
                if (right) right[offset + i] = value;
            }
            offset += frames;
            if (this.playing) this.samplesUntilStep -= frames;
        }
        return true;
    }
}

registerProcessor('trx-processor', TrxProcessor);
