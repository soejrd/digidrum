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
assert.deepEqual(Array.from(posted.at(-1).defaults[0].controls), [64, 64, 64, 0, 0, 0, 0, 0]);
assert.deepEqual(Array.from(posted.at(-1).defaults[1].controls), [34, 13, 0, 64, 127, 0, 104, 85]);
assert.equal(posted.at(-1).defaults[2].count, 5);
assert.equal(posted.at(-1).defaults.length, 16);
assert.deepEqual(Array.from(posted.at(-1).defaults[8].controls),
    [45, 65, 58, 48, 42, 35, 45, 25]);
assert.equal(posted.at(-1).defaults[13].count, 7);
assert.equal(posted.at(-1).defaults[15].count, 7);
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

assert.equal(posted[0].tweakDescriptors[0].name, 'pitch_percent');
assert.ok(posted[0].tweakValues.slice(0, 7).every(value => value === 100));
await processor.onMessage({type: 'kind', kind: 0});
await processor.onMessage({type: 'trigger'});
const baseline = block();
await processor.onMessage({type: 'kind', kind: 0});
await processor.onMessage({type: 'tweak-set', index: 0, value: 150});
assert.equal(posted.at(-1).value, 150);
await processor.onMessage({type: 'trigger'});
assert.notDeepEqual(block(), baseline, 'TRX pitch tweak changes rendered sound');
await processor.onMessage({type: 'kind', kind: 1});
await processor.onMessage({type: 'kind', kind: 0, tweaks: [150]});
assert.equal(posted.at(-1).values[0], 150, 'TRX tweak survives a machine switch');
await processor.onMessage({type: 'tweak-set', index: 1, value: 10});
await processor.onMessage({type: 'trigger'});
assert.ok(block().every(Number.isFinite), 'shortest TRX-B2 decay stays valid');

await processor.onMessage({type: 'kind', kind: 2});
const hatFields = posted.at(-1).descriptors.map(desc => desc.name);
const hatIndex = name => hatFields.indexOf(name);
async function hatBlock(hpBoost, lpBoost, hpQ = 200, lpQ = 200) {
    await processor.onMessage({type: 'kind', kind: 2});
    for (let index = 0; index < 8; index++)
        await processor.onMessage({type: 'control', index,
            value: posted[0].defaults[2].controls[index]});
    for (const [name, value] of [
        ['hp_eq_boost_percent', hpBoost], ['lp_eq_boost_percent', lpBoost],
        ['hp_eq_q_x100', hpQ], ['lp_eq_q_x100', lpQ]
    ]) await processor.onMessage({type: 'tweak-set', index: hatIndex(name), value});
    await processor.onMessage({type: 'trigger'});
    return block();
}
const dryHat = await hatBlock(0, 0);
assert.notDeepEqual(await hatBlock(200, 0), dryHat, 'HPF EQ boosts independently');
assert.notDeepEqual(await hatBlock(0, 200), dryHat, 'LPF EQ boosts independently');
assert.notDeepEqual(await hatBlock(200, 0, 800), await hatBlock(200, 0, 80),
    'HPF Q changes its band');

for (let kind = 0; kind < 16; kind++) {
    await processor.onMessage({type: 'kind', kind});
    for (let index = 0; index < 8; index++)
        await processor.onMessage({type: 'control', index,
            value: posted[0].defaults[kind].controls[index]});
    await processor.onMessage({type: 'trigger'});
    let peak = 0;
    for (let i = 0; i < 8; i++) {
        for (const sample of block()) peak = Math.max(peak, Math.abs(sample));
    }
    assert.ok(peak > 0.005, `machine ${kind} was silent`);
}

await processor.onMessage({type: 'kind', kind: 0});
await processor.onMessage({type: 'step-active', index: 0, active: true});
await processor.onMessage({type: 'play'});
assert.ok(block().some(sample => Math.abs(sample) > 0.01));
assert.ok(posted.some(message => message.type === 'step' && message.index === 0));
for (let i = 0; i < 50; i++) block(); // > one sixteenth at 120 BPM
assert.ok(posted.some(message => message.type === 'step' && message.index === 1));
await processor.onMessage({type: 'stop'});
console.log('ok: all 16 TRX and EFM selectors, WASM voice, stereo, and step timing');
