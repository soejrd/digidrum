document.addEventListener('DOMContentLoaded', function() {
    // Initialize all knobs
    const knobWrappers = document.querySelectorAll('.knob-wrapper');
    knobWrappers.forEach(wrapper => {
        const paramName = wrapper.dataset.param;
        const initialValue = parseInt(wrapper.dataset.value);
        
        // Find the knob elements within this wrapper
        const knobContainer = wrapper.querySelector('.knob-container');
        const ringFill = knobContainer.querySelector('.ring-fill');
        const knobIndicatorContainer = knobContainer.querySelector('.knob-indicator-container');
        
        // Set initial rotation based on value (0-127 maps to -140 to 140 degrees)
        // But the user's example starts at 140 degrees for 0? Let me check...
        // Looking at the JS: knob starts at rotate(140deg) and ring-fill starts at conic-gradient(var(--accent) 140deg...)
        // So 140 degrees = 0 value, and it can go from -140 to 140 degrees
        // Let's map 0-127 to -140 to 140 degrees
        const rotation = mapValueToRotation(initialValue, 0, 127, -140, 140);
        
        // Apply initial styles
        knobIndicatorContainer.style.transform = `rotate(${rotation}deg)`;
        updateRingFill(ringFill, rotation);
        
        // Store initial rotation for pointer events
        let lastRot = rotation;
        
        // Pointer down event
        knobContainer.addEventListener('pointerdown', (event) => {
            const startY = event.clientY;
            
            const onPointerMove = (moveEvent) => {
                const delta = startY - moveEvent.clientY;
                let currentY = lastRot + delta * 1.5; // speed = 1.5 from user's code
                
                // Clamp to max rotation
                const maxRot = 140;
                if (currentY > maxRot) currentY = maxRot;
                if (currentY < -maxRot) currentY = -maxRot;
                
                // Update knob indicator rotation
                knobIndicatorContainer.style.transform = `rotate(${currentY}deg)`;
                
                // Update ring-fill background
                updateRingFill(ringFill, currentY);
                
                // Update parameter value and audio
                const value = mapRotationToValue(currentY, -140, 140, 0, 127);
                wrapper.dataset.value = Math.round(value);
                
                // Update audio utils if available
                if (window.audioUtils && window.audioUtils.setParam) {
                    window.audioUtils.setParam(paramName, Math.round(value));
                }
            };
            
            const onPointerUp = () => {
                document.removeEventListener('pointermove', onPointerMove);
                document.removeEventListener('pointerup', onPointerUp);
                lastRot = parseFloat(knobIndicatorContainer.style.transform.replace('rotate(', '').replace('deg)', '')) || lastRot;
            };
            
            document.addEventListener('pointermove', onPointerMove);
            document.addEventListener('pointerup', onPointerUp);
        });
    });
    
    // Helper functions
    function mapValueToRotation(value, inMin, inMax, outMin, outMax) {
        return (value - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
    
    function mapRotationToValue(rotation, inMin, inMax, outMin, outMax) {
        return (rotation - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
    
    function updateRingFill(ringFill, rotation) {
        if (rotation > 0) {
            ringFill.style.background = `conic-gradient(var(--accent) ${rotation}deg, rgba(255,255,255,0.0) 0 360deg, var(--accent) 0deg)`;
        } else {
            ringFill.style.background = `conic-gradient(var(--accent) 0deg, rgba(255,255,255,0.0) 0 ${360 + rotation}deg, var(--accent) 0deg)`;
        }
    }
    
    // Tempo control
    const tempoInput = document.getElementById('tempo');
    const tempoValue = document.getElementById('tempo-value');
    tempoValue.textContent = tempoInput.value;
    tempoInput.addEventListener('input', function() {
        tempoValue.textContent = this.value;
        // Update audio tempo if set up
        if (window.audioUtils && window.audioUtils.setTempo) {
            window.audioUtils.setTempo(this.value);
        }
    });
    
    // Sequencer steps
    const steps = document.querySelectorAll('.step');
    const playhead = document.querySelector('.playhead');
    let currentStep = 0;
    let isPlaying = false;
    let intervalId = null;
    
    steps.forEach((step, index) => {
        step.addEventListener('click', function() {
            this.classList.toggle('active');
            // If audio utils exist, update the step state
            if (window.audioUtils && window.audioUtils.setStepActive) {
                window.audioUtils.setStepActive(index, this.classList.contains('active'));
            }
        });
    });
    
    // Play and Stop buttons
    const playBtn = document.getElementById('play-btn');
    const stopBtn = document.getElementById('stop-btn');
    playBtn.addEventListener('click', function() {
        if (isPlaying) return;
        isPlaying = true;
        currentStep = 0;
        updatePlayhead();
        // Start the sequencer interval
        const bpm = parseInt(tempoInput.value);
        const interval = 60000 / bpm / 4; // sixteenth notes
        intervalId = setInterval(() => {
            currentStep = (currentStep + 1) % 16;
            updatePlayhead();
            // Trigger note for active steps
            if (window.audioUtils && window.audioUtils.triggerStep) {
                window.audioUtils.triggerStep(currentStep);
            }
        }, interval);
        // Also start audio context if not resumed
        if (window.audioUtils && window.audioUtils.start) {
            window.audioUtils.start();
        }
    });
    stopBtn.addEventListener('click', function() {
        isPlaying = false;
        clearInterval(intervalId);
        intervalId = null;
        if (window.audioUtils && window.audioUtils.stop) {
            window.audioUtils.stop();
        }
    });
    
    function updatePlayhead() {
        const stepWidth = 22 + 6; // step width + gap
        const left = currentStep * stepWidth;
        playhead.style.left = `${left}px`;
    }
    
    // Machine dropdown (placeholder)
    const machineSelect = document.getElementById('machine-select');
    machineSelect.addEventListener('change', function() {
        console.log('Machine selected:', this.value);
        // Here you could load different parameters for different machines
        // For now, just log
    });
    
    // Initialize audio utilities
    window.audioUtils = new AudioUtils();
});

// Simple audio utility class (keeping the existing one)
class AudioUtils {
    constructor() {
        this.audioContext = null;
        this.oscillators = {};
        this.gainNodes = {};
        this.noiseBuffers = {};
        this.distortion = {};
        this.params = {
            pitch: 64,
            decay: 64,
            ramp: 64,
            hold: 0,
            tick: 0,
            noise: 0,
            dirt: 0,
            dist: 0
        };
        this.stepsActive = new Array(16).fill(false);
        this.tempo = 120; // BPM
        this.isStarted = false;
    }
    
    start() {
        if (this.isStarted) return;
        this.audioContext = new (window.AudioContext || window.webkitAudioContext)();
        this.isStarted = true;
        console.log('Audio context started');
    }
    
    stop() {
        if (!this.isStarted) return;
        this.audioContext.close();
        this.isStarted = false;
        // Clear oscillators
        for (let key in this.oscillators) {
            this.oscillators[key].stop();
            delete this.oscillators[key];
        }
        this.oscillators = {};
        this.gainNodes = {};
    }
    
    setParam(name, value) {
        this.params[name] = value;
        // Optional: update any active voices
    }
    
    setTempo(bpm) {
        this.tempo = bpm;
    }
    
    setStepActive(index, active) {
        this.stepsActive[index] = active;
    }
    
    triggerStep(stepIndex) {
        if (!this.isStarted || !this.stepsActive[stepIndex]) return;
        
        // Create a simple voice
        const now = this.audioContext.currentTime;
        
        // Oscillator
        const oscillator = this.audioContext.createOscillator();
        // Map pitch param to frequency: 0-127 -> C2 (65.41Hz) to C6 (1046.50Hz)
        const pitchValue = this.params.pitch;
        const frequency = 65.41 * Math.pow(2, (pitchValue - 0) * (Math.log2(1046.50/65.41) / 127));
        oscillator.frequency.setValueAtTime(frequency, now);
        oscillator.type = 'sine'; // we can add other waveforms based on params
        
        // Gain node for envelope
        const gainNode = this.audioContext.createGain();
        gainNode.gain.setValueAtTime(0, now);
        
        // Envelope: attack, decay, sustain, release
        // We have decay, hold, ramp (maybe sustain), etc.
        // Let's map:
        // attack: fixed 0.01s
        // decay: map decay param 0-127 -> 0.01 to 2s
        // sustain: map ramp param 0-127 -> 0 to 1 (sustain level)
        // release: fixed 0.1s
        const attackTime = 0.01;
        const decayTime = 0.01 + (this.params.decay / 127) * 1.99; // 0.01 to 2.0
        const sustainLevel = this.params.ramp / 127; // 0 to 1
        const releaseTime = 0.1;
        
        // Envelope
        gainNode.gain.linearRampToValueAtTime(1, now + attackTime); // attack
        gainNode.gain.linearRampToValueAtTime(sustainLevel, now + attackTime + decayTime); // decay to sustain
        // We'll hold sustain until note ends, but we don't have note length from sequencer.
        // For simplicity, we'll release after a fixed time (e.g., 0.5s) or based on decay?
        // Let's release after decay time + sustain time? We'll just release after decay + 0.2s.
        const totalTime = attackTime + decayTime + 0.2;
        gainNode.gain.setValueAtTime(sustainLevel, now + totalTime); // hold sustain
        gainNode.gain.linearRampToValueAtTime(0, now + totalTime + releaseTime); // release
        
        // Connect
        oscillator.connect(gainNode);
        gainNode.connect(this.audioContext.destination);
        
        // Start oscillator
        oscillator.start(now);
        oscillator.stop(now + totalTime + releaseTime);
        
        // Store for potential cleanup (optional)
        const voiceId = `${now}-${Math.random()}`;
        this.oscillators[voiceId] = oscillator;
        this.gainNodes[voiceId] = gainNode;
        
        // Clean up after sound ends
        setTimeout(() => {
            if (this.oscillators[voiceId]) {
                delete this.oscillators[voiceId];
                delete this.gainNodes[voiceId];
            }
        }, (totalTime + releaseTime) * 1000 + 100);
    }
}