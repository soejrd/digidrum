import assert from 'node:assert/strict';
import fs from 'node:fs';
import vm from 'node:vm';

const posted = [];
let Processor;
class FakeAudioWorkletProcessor {
    constructor() {
        this.port = {postMessage: message => posted.push(message)};
    }
}
vm.runInNewContext(fs.readFileSync('website/trx-worklet.js', 'utf8'), {
    AudioWorkletProcessor: FakeAudioWorkletProcessor,
    registerProcessor: (name, klass) => {
        assert.equal(name, 'trx-processor');
        Processor = klass;
    },
    WebAssembly,
    Int32Array,
    Math,
    Number,
    Boolean,
    sampleRate: 48000
});

const processor = new Processor();
const file = fs.readFileSync('website/trx-synth.wasm');
const bytes = file.buffer.slice(file.byteOffset, file.byteOffset + file.byteLength);
await processor.onMessage({type: 'wasm', bytes});
assert.equal(posted.at(-1).type, 'ready');
assert.deepEqual(Array.from(posted.at(-1).defaults[1].controls), [64, 64, 64, 0, 0, 0, 0, 0]);
assert.deepEqual(Array.from(posted.at(-1).defaults[2].controls), [34, 13, 0, 64, 127, 0, 104, 85]);
assert.equal(posted.at(-1).defaults[3].count, 5);
assert.equal(processor.capacity, 128);

function block() {
    const left = new Float32Array(128);
    const right = new Float32Array(128);
    processor.process([], [[left, right]]);
    assert.deepEqual(left, right);
    for (const sample of left) assert.ok(sample >= -1 && sample <= 1);
    return left;
}
assert.ok(block().every(sample => sample === 0));
await processor.onMessage({type: 'trigger'});
const hit = block();
assert.ok(hit.some(sample => Math.abs(sample) > 0.01));
assert.ok(block().some(sample => sample !== 0)); // voice survives the block

for (let kind = 0; kind <= 8; kind++) {
    await processor.onMessage({type: 'kind', kind});
    await processor.onMessage({type: 'trigger'});
    let peak = 0;
    for (let i = 0; i < 8; i++) {
        for (const sample of block()) peak = Math.max(peak, Math.abs(sample));
    }
    assert.ok(peak > 0.005, `TRX kind ${kind} was silent`);
}

await processor.onMessage({type: 'kind', kind: 1});
await processor.onMessage({type: 'step-active', index: 0, active: true});
await processor.onMessage({type: 'play'});
assert.ok(block().some(sample => Math.abs(sample) > 0.01));
assert.ok(posted.some(message => message.type === 'step' && message.index === 0));
for (let i = 0; i < 50; i++) block(); // > one sixteenth at 120 BPM
assert.ok(posted.some(message => message.type === 'step' && message.index === 1));
await processor.onMessage({type: 'stop'});
console.log('ok: all nine TRX selectors, WASM voice, stereo, and step timing');
