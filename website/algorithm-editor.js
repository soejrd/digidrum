import {Pane} from 'https://cdn.jsdelivr.net/npm/tweakpane@4.0.5/dist/tweakpane.min.js';

const common = [
    'c_min_hz', 'c_hz_range', 'm_min_hz', 'm_hz_range',
    'amp_min_ms', 'amp_span_ms', 'depth_mult'
];
const modTime = ['mod_min_ms', 'mod_span_ms'];
const sweep = ['sweep_max_hz', 'ramp_min_ms', 'ramp_span_ms'];
const aux = ['aux_min_ms', 'aux_span_ms'];
const highpass = ['hp_min_hz', 'hp_hz_range'];
const metallic = ['ratio_0', 'ratio_1', 'ratio_2', 'ratio_3'];
const used = [
    ['c_min_hz', 'c_hz_range', ...modTime, ...sweep,
        'amp_min_ms', 'amp_span_ms', 'bd_mod_ratio_min_q8',
        'bd_mod_ratio_span_q8', 'bd_index_max_q8', 'bd_mod_attack_ms',
        'fb_depth_mult',
        'phase_offset_q2'],
    [...common, ...modTime, ...aux, 'noise_gain_mult', 'fb_depth_fix', ...highpass],
    [...common, ...modTime, ...sweep, 'fb_depth_fix', 'snap_gain_mult',
        'hp_frac_num', 'hp_frac_den', 'phase_offset_q2'],
    [...common, ...modTime, ...aux, 'clap_max_count', 'clap_period',
        'fb_depth_fix', ...highpass],
    ['c_min_hz', 'c_hz_range', 'rim_mod_ratio', 'rim_mod_offset',
        'c2_min_hz', 'c2_hz_range', 'm2_offset_hz', 'm2_hz_per_control',
        'amp_min_ms', 'amp_span_ms', 'mod_fixed_ms', ...aux, 'depth_mult',
        'fb_depth_fix', 'noise_gain_mult', 'snap_gain_mult', ...highpass],
    [...common, ...modTime, 'cb_ratio_percent', 'cb_aux_min_ms',
        'cb_aux_divisor', 'fb_depth_mult', 'snap_gain_mult'],
    [...common, ...modTime, ...metallic, 'fb_depth_mult',
        'trem_depth_mult', 'trem_freq_min_hz', 'trem_freq_range', 'hp_fixed_hz'],
    [...common, ...modTime, ...metallic, 'fb_depth_mult', ...highpass]
];

const trxUsed = [
    ['pitch_percent', 'decay_percent', 'transient_percent', 'sweep_percent',
        'noise_percent', 'body_percent'],
    ['pitch_percent', 'decay_percent', 'transient_percent', 'sweep_percent',
        'noise_percent', 'body_percent'],
    ['pitch_percent', 'decay_percent', 'noise_percent', 'body_percent', 'metal_percent'],
    ['pitch_percent', 'decay_percent', 'noise_percent', 'body_percent', 'metal_percent'],
    ['pitch_percent', 'decay_percent', 'transient_percent', 'noise_percent',
        'body_percent', 'metal_percent'],
    ['pitch_percent', 'decay_percent', 'transient_percent', 'body_percent'],
    ['pitch_percent', 'decay_percent', 'transient_percent', 'sweep_percent',
        'noise_percent', 'body_percent'],
    ['pitch_percent', 'decay_percent', 'transient_percent', 'noise_percent', 'body_percent']
];

const groups = [
    ['Pitch and sweep', /^(pitch_|sweep_)/],
    ['Decay and attack', /^(decay_|transient_)/],
    ['Source balance', /^(noise_|body_|metal_)/],
    ['Pitch', /^(c\d?_.*hz|m\d?_.*hz|rim_|cb_ratio|sweep_|ratio_|phase_|bd_mod_ratio)/],
    ['Envelopes', /^(amp_|mod_.*ms|ramp_.*ms|aux_|cb_aux_|clap_|bd_mod_attack)/],
    ['Modulation and mix', /^(depth_|fb_|noise_|snap_|bd_index)/],
    ['Filter', /^hp_/],
    ['Tremolo', /^trem_/]
];

function groupName(field) {
    return groups.find(([, pattern]) => pattern.test(field))?.[0] || 'Other';
}

function fieldLabel(kind, name) {
    if (kind < 8) {
        if (name === 'body_percent') return kind === 0 ? 'body level %' : 'output level %';
        if (name === 'transient_percent') return kind === 0 ? 'tick level %' : 'transient length %';
        if (name === 'decay_percent') return 'decay time %';
    }
    return name.replaceAll('_', ' ');
}

export class AlgorithmEditor {
    constructor(container, onChange) {
        this.container = container;
        this.onChange = onChange;
        this.pane = null;
        this.kind = -1;
        this.descriptors = [];
        this.state = {};
    }

    show(kind, descriptors, values) {
        this.pane?.dispose();
        this.kind = kind;
        this.descriptors = descriptors;
        this.state = Object.fromEntries(descriptors.map((desc, index) =>
            [desc.name, values[index]]));
        this.pane = new Pane({container: this.container});
        const relevant = new Set(kind < 8 ? trxUsed[kind] : used[kind - 8]);
        for (const [title] of groups) {
            const entries = descriptors.map((desc, index) => ({...desc, index}))
                .filter(desc => relevant.has(desc.name) && groupName(desc.name) === title);
            if (!entries.length) continue;
            const folder = this.pane.addFolder({title, expanded: title === 'Pitch' || title === 'Pitch and sweep'});
            for (const desc of entries) {
                folder.addBinding(this.state, desc.name, {
                    min: desc.min, max: desc.max, step: 1,
                    label: fieldLabel(kind, desc.name)
                }).on('change', event => this.onChange(desc.index, Math.round(event.value)));
            }
        }
    }

    update(values) {
        if (!this.pane) return;
        let changed = false;
        this.descriptors.forEach((desc, index) => {
            if (this.state[desc.name] !== values[index]) {
                this.state[desc.name] = values[index];
                changed = true;
            }
        });
        if (changed) this.pane.refresh();
    }
}
