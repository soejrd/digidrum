Elektron Machinedrum Tips and Tricks v1.01 

# **Index** 

## Machinedrum Sequencing 

|Setting individual accent amounts for each track<br>Copying just one note's parameter locks in Grid Record Mode|1|
|---|---|
|<br>Using the Machinedrum to sequence external gear||
|Changing the default notes numbers on the Machinedrum (IE, from c1-c3 to c3-c5):|2|
|Realtime erase|3|
|Can the Machinedrum pads transmit velocity or aftertouch?<br>||
|A different way of sequencing MIDI Machines<br>||
|Cycling/Looping Patterns||
|<br>Realtime recording of external synths controllers onto MIDI Machine tracks|4|
|To disable the Machinedrum from transmitting MIDI-Data (MIDI clock) even when stopped||
|Making the Machinedrum do triplets||
|<br>Uses for Parameter Lock Slides||
|Neat trick for switching from Classic to Extended Modes|5|
|Creating time signatures like 12/8||
|Disabling MIDI clock send and receive||
|Machinedrum Synthesis||
|Creating thick 80's drum sounds||
|Granular technique|6|
|ctr-al and ctr-8p machines||
|Creating basslines with the Machinedrum|7|
|Bass drum tips|8|
|<br>Snare drum tips|9|
|Mimicing snare drum 'flam' sounds|10|
|Making snare rolls||
|Making 909 style hihats||
|Crunchy drum sounds|11|
|Making tuned sounds in the E12 kit||
|Echo and Compressor interactions||
|LFO Speed Settings|12|
|Emulating synth envelopes||
|Using the Machinedrum mainly for melodic material|13|
|Emulating string sounds with the Machinedrum:||
|EFM-HH as a violin type sound||
|Saving individual drums|14|
|Using effects from more than one track||
|Performance Tips||
|Using MIDI Program changes|15|
|Using the RAM Machines||
|More on UW ROM features|16|
|Machinedrum with DJ decks<br>|17|
|General live playing tips||
|<br>Using the inputs in stereo|18|
|Using the Machinedrum as an effects unit<br>General Machinedrum tips||
|Did I see Elvis?|19|
|Serial Number = Machinedrum number?||
|Using the Compressor||
|<br>Tempo syncing the LFO's on Machinedrum and mnm|20|
|Pops and crackles||
|Cases|21|
|Replacing the power supply|22|
|Removing the knobs and data encoder||
|Test Mode info|23|
|New features that are not in the manual yet||



# **Elektron Machinedrum Tips and Tricks** 

v1.01 

This document was created while I was browsing the Elektron Users Forums and Yahoo Group Mailing List looking for new ideas for things to try with my recently acquired SPS-1UW. Some of these are new tips, some are just clarifications of features explained in the manual already. I have attempted to the best of my ability to make sure that each author is properly given credit for their tips. However, if you spot an error or do not wish to have your name in this document, please let me know and I will fix the problem immediately. 

In all cases, tips were quoted using the authors yahoo name, or their screen name from the forum. This was done to protect each author's right to privacy, yet still provides a way for any reader to attempt to contact the author should they wish to do so. The words "list" and "bbb" after all names denote which tips came from the mailing list, versus which tips came from the message board. 

Compiled by Tarekith on February 15, 2006. 

## Machinedrum Sequencing 

Setting individual accent amounts for each track 

The amount of accent is shared between tracks but accent place is individual per track. So you can set accent to 8, put different accent points to different tracks, but they will all be accented by factor of 8 (at different points). And beyond this you always have individual control of each hit volume, length, pitch and so on with parameter locks. 

### - toni bbb 

I have never used the accents on the Machinedrum - I just parameter lock in a volume tweak. Its faster for me. 

### - milkmansnd bbb 

Copying just one note's parameter locks in Grid Record Mode 

In grid edit, hold a trig key and then hit the play/copy button, then hold a different trig and hit paste, it duplicates all parameter locks, quite handy. 

-ggoodwin bbb 

Using the Machinedrum to sequence external gear 

First of all, yes, the Machinedrum can sequence but its not nearly as intuitive as the live sequencer, or even the one in the mono machine for that matter. Load a MIDI machine, and you can assign a note to each step one by one using p-locks. 

-milkmansnd bbb 

1 

You choose the MIDI channel you want to control when selecting the MIDI machine. You got 16 different ones. One for each channel. 

### - Rui Peixoto list 

One good plus is using the Machinedrum to tweak another patch in the virus multi-mode. The Machinedrum lfo's can tweak the virus. You have to set your most wanted CC#s in the Machinedrum MIDI machine. Using the envelope input machine is something I have been discovering, too. 

### - endlessnessisticman list 

You're using the Machinedrum to "play" your synth, right? That is, you've assigned a MIDI machine in the Machinedrum to control the synth note sequence. Assuming this, all you have to do is decide what are your six most favored MIDI controls that you'd like to tweak on the virus and assign them to CC's on the MIDI machine. Then tweak those while the pattern is recording and it will play them back correctly (assuming your changes fell where triggering was being done). 

Now, you'd like to do this in song mode. This IS possible, but the solution is ugly :-) for example, if you're talking about having a pattern repeat 8 times while you tweak the parameters for it...you can have the Machinedrum record these changes as described above; BUT you would have to use 8 versions of the "base" pattern and have the parameter changes effect the pattern that they fall on. make sense? 

BTW, if you want more than six CC controls for a given MIDI channel (for example on your virus) then you can just use another Machinedrum MIDI machine assigned to the same channel. this feature can also be used to increase the polyphony of the Machinedrum's external MIDI triggering ability. (since Machinedrum MIDI machines only use monophonic triggering.) 

### - Joe list 

The Machinedrum has sequencing possibilities, but it's not like a regular soft sequencer. There are 16 parts in total, and they can be assigned to either internal Machinedrum sounds or MIDI sequencing parts (there are other possibilities as well, but they're not relevant here). Which means you can control a total of 16 external synths. 

A MIDI sequencing part is basically monophonic (one trigger at a time, one basic note), but each step can play up to 3 notes simultaneously (triggered as one note). Which means that if you like to play big chords, you're in trouble. You would need a multitimbral synth, and set it up with the same sound on several channels. Not ideal. 

It's not the most versatile of MIDI sequencers, but it does have its strengths, like the ability to set all parameters separately for each step (like CCs, eight per track). If you're using something like a modular analog synth with a MIDI-to-CV converter, you'll have a serious step sequencer. I've tried it with a Korg MS-20, and it works. 

### - Nils Odegaard list 

Changing the default notes numbers on the Machinedrum (IE, from c1-c3 to c3-c5) 

These can be changed in the global settings area. Press function+[global] and select the slot that you want to edit. Press enter and select the MIDI section, map editor submenu. From there, you can change all of the track and pattern mappings. 

### - malyn bbb 

2 

### Realtime erase 

while in rec+play mode, hold exit/no and the trig key you want to erase. It will play the trig this time through the loop, but you will not hear it next time. 

### – ggoodwin bbb 

### Can the Machinedrum pads transmit velocity or aftertouch? 

No and no. However, you can have different volumes, decay, pitch or any parameter for each individual step with parameter locks, so it's no big deal achieving more "human" like feel for your sequences. 

### - Flavio Alvarez list 

If its absolutely necessary, just bring along an oxygen 8 or other inexpensive controller that supports pressure sensitivity, and trigger The pads from there. In fact, with the oxygen, you'll even have 8 Extra knobs you can use, too. As far as I can remember, machinedrum Still responds to pressure sensitivity (mapped to volume) this way. 

### – Adam Watson list 

### A different way of sequencing MIDI Machines 

Something a friend turned me on to, which I had never thought of for some reason: I was used to using one midi machine per synth I was sequencing. While we were both punching up patterns, he configured his Machinedrum with multiple midi machines, all transmitting on the same channel, and then set separate notes for each. This way you can enter Real-time Record and play eight or more notes into the sequence, just like a piano. 

### - jason vernon starnes list 

Yeah, and as an extension of that, you could a make two octaves, one MIDI Machine on each track, tweak out a pattern live and then record it down to another sequence and repeat. 

### – jbeard list 

If your looking for a step sequencer capable of driving external MIDI synths and samplers then the Machinedrum is one of the few choices that has it right with regard to tweaking while the sequence is playing. If you have been frustrated by the way the mc-series grooveboxes do this, or worse the rm1x, or even the emu xx-7 family you will find the Machinedrum a breath of fresh air. Only more esoteric (and expensive) things can work as intuitively and Interactively. 

### – drk list 

### Cycling/Looping Patterns 

1. You press function and a different pattern you'll get the single arrow and no cycling back to the old one. 

2. You hold function and more than one pattern button you'll get the double arrow and cycling multiple patterns in the order you pressed the pattern buttons. 

3 

### - endlessnessisticman list 

You can loop some patterns (I don't mean song mode) if you hold the bank button and then hold the patterns you want to be played in a loop. e.g. push and hold d+3+4+5+6 = Machinedrum plays 3-45-6-3-4-5-6-... the quantization depends on the pattern-quantization so you can play 16th-32th6th-whateverth. If you don't want to compose a song, this is a nice feature. Also very good for live use. 

### - rolandsh32 list 

### Realtime recording of external synths controllers onto MIDI Machine tracks 

As far as controlling the parameter values themselves from the Supernova (or any synth), I wasn't able to tweak the knobs and get the Machinedrum to record them. I had to map the Machinedrum knobs to the Supernova MIDI control values that I wanted to change and tweak from The Machinedrum. 

### - alan cannistraro list 

### To disable the Machinedrum from transmitting MIDI-Data (MIDI clock) even when stopped 

Global menu->sync->out->off 

### - Federico Ciapi list 

### Making the Machinedrum do triplets 

You can achieve triplets if you set the pattern length to 12 or 24. 

Hmm, on second thought, all that would do is change the time signature from 4/4 to 3/4... not quite the same thing as triplets, though it can "feel" the same in some situations... 

### - Federico Ciapi list 

You can use the swing-grid function to do the triplet (or did you mean a triple, like a waltz or something?). Put the grid steps on 3, 7, 11, 15... set the swing percentage to 80%. This is a Triol groove. 

### - John Whiting list 

### Uses for Parameter Lock Slides 

I use it sometimes when I recorded parameter locks but I don't want them to jump around. With slide you get a smooth parameter glide. For instance, if you want panning to go from left to right in a smooth fashion you simply program a lock left pan on the first note and a firm right on the last. When you select all triggers in slide the panning will go slow from left to right instead of jumping. 

### - Ronald list 

4 

### Neat trick for switching from Classic to Extended Modes 

Forgot to mention one interesting use of it: Switch to classic mode and then pick up pattern that would play with different kit in extended mode. Hit play and it runs with a 'wrong kit', now hit the extended mode (while it's playing) and you get wonderful noises out of Machinedrum when it changes to right kit in the fly!! 

### - tahvenaine2002 list 

### Creating time signatures like 12/8 

In song mode, you can choose how many steps of the pattern you want to play. That way is possible to use those time signatures. 

### - Federico Ciapi list 

Machinedrum's Scale setup can give a time "signature" of 12/16. Turn the tempo down from 120 to 60, and there you have it. 12 beats per measure, every 1/8 note is a "beat", I.e. 12/8. Of course, you're left with half note resolution, which isn't all that nice. 

A better way to do this is to leave the tempo set at for example 120, and choose 24/32 in the scale setup. You get the same end result, but with higher resolution (quarter note) between beats. 

### - oldmanfury list 

### Disabling MIDI clock send and receive 

That's basically what the mode Sync Tempo IN ON in the global edit window will do. Be sure Sync Crtl IN is OFF. 

MIDI Sync OUT switches between transmission of Start, Stop, Continue and MIDI Timing Clocks OUT of the Machinedrum. In case it's page 57 of the 1.12 manual. 

### - lepetitmartien list 

### Machinedrum Synthesis 

### Extreme distortion on the Machinedrum: 

Route a sep. out back in and use the gain stage on the ext. input machine. Fucking nosebleed dutch hell death. No problem with the industrial sounds. 

### -D.E list 

### Creating thick 80's drum sounds 

This can totally happen with the Machinedrum - even that tight comp'd sound can be had with the dynamix. You definitely have the right machine for the job - play with the dynamix, reverb, and a combo of the fm and trx machines - maybe a little noise machine to double the snare hits, and you 

5 

will have that sound nailed. 

I usually work the output gain and threshold on the dynamix to make everything hit as hard as possible and be as "up front" sounding as I can get it - plus, you will see the elektron box grow louder and stronger than everything else in your rig without even touching the potential or the ceiling of headroom. 

### - milkmansnd bbb 

...and gate it! The gate parameter in the Machinedrum reverb is perfect for this... Instant 80s 

### - anon bbb 

Yes, a short gate time with fm machines and little compression is good for 80's sounds 

### - mike bbb 

### Granular technique 

Cheap sample loaded into my mduw.. (i.e., saying "wisefire") Put strt on 127 and rtrg on full. Rtim on something in the vicinity of 25. Leave LFO on random, and the right on exponential slope, depth on full. And then adjust speed so that the full rtrg is filled. And the finally the param to lfo is strt. This way you can get a very cheap but fun little timestretcher and/or pitchbender. 

### - wisefire bbb 

### ctr-al and ctr-8p machines 

I'm not in front of my Machinedrum at the moment so bear with me...the ctrl-all machine basically sends any param locks that it receives to the corresponding knob on all other tracks. Parameters on page 2 and 3 are the same for all tracks, but the parameters on page 1 vary depending on what machine you have on that track. So if you crank knob 4 on page 1 of the ctrl-all machine, you will be changing whatever parameter is assigned to knob 4 for every other track. 

You were correct to notice that it is destructive. It changes the setting of each track. Also, if you move the knob down 20 then up 20, you can't be certain that all tracks will be back to their original setting (for example, if one of the tracks had a value of 10 in that position to start with, it's final value would be 20 since moving down would take it to 0 then moving back up 20 leaves it at 20). 

The ctrl-all machine seems to be more useful as a chaos-inducer rather than a controlled sound sculpture tool. You will probably be using load-kit a lot with this machine. 

Another related, undocumented feature: you can achieve a similar effect without loading the ctrl-all machine to a track, by holding "function" while turning a knob...the knob movement will be sent to all tracks. The advantage to using the ctrl-all machine instead of function+knob is that you can sequence and param-lock with the ctrl-all machine. 

### -ggoodwin bbb 

Holding function changes parameters for all machines simultaneously - from my understanding, the ctr machines automate this like parameter locks would to a single sequencer. So, lets say you want the entire pattern to drop in pitch - this could either be done manually - by holding the function knob - or you can automate it with the ctr machines. Its prime directive is to control all, thus the name. 

6 

### - milkmansnd bbb 

The ctr-8p machine allows you to assign whatever you want to the first 8 parameters of the machine (I.e. On the synthesis page). You can take any parameter from any other loaded machine and assign it to one of these. Which means that you can stare at one control page and tweak completely different and independent parameters for several different machines without having to change pages or active tracks. In addition, you can sequence changes to these parameters via parameter locks. What's great about this is that the locks on a ctr machine track (for this or for the ctr-al) don't trig anything; they just change parameters. Which is great because this is the only feature that the mono machine's sequencer has over the Machinedrum's and these machines make it possible. 

The ctr-al machine changes the respective parameter on every track. It's the same as holding function while changing a parameter on a track. The difference here is that you can sequence these global parameter changes through parameter locks. 

### - chiasticon bbb 

CTR stands for "ConTRol". The Machinedrum does not have the ability to place parameter locks for a track on a step which does not have a trigger on it, so these CTR machines fill a gap in the Machinedrum's functionality, by letting you use a track to do nothing but "send" parameter changes to other tracks. The mono machine, of course, can already put parameter locks on any step you want. 

The exciting part for me is how the CTR-8P (for "8 Parameter") machine centralizes control of many parameters on different tracks onto one screen. Now you can have the pitch from the bass drum, the interference frequency from your snare drum, and the decay settings for your hats and things all on one screen, for easy use during live performance, without using up a lot of hand movement changing active track. That is why CTR-8P is cool. 

The other CTR machine is CTR-AL (for "ALL tracks"). You do not choose which specific tracks the control changes on this machine go to - changing a parameter on CTR-AL changes that same parameter on EVERY track. Using CTR-AL, you can create some interesting kit-wide effects on drum tracks which would be difficult to achieve otherwise, sort of like how the BBOX machine's synthesis parameters affect every drum sample on a BBOX track. 

The danger of CTR machines is that they alter the non-locked parameter settings in the active kit, so if you save the kit while it is being altered by a CTR machine track, the changes that the CTR machine made are saved! You have to make a habit of saving a kit directly after adding CTR machines to it, before you do anything. 

So, the feature of having a collection of arbitrary parameters from all tracks on one screen is very, very useful on a 16 track machine such as the Machinedrum, in addition to fulfilling the trig-less lock function. Using 1 track out of a 6 track Monomachine just for controls would be somewhat less exciting. I would rather the Monomachine got a new FX machine, like a ring modulator or distortion synthesizer. 

### -Andrew Deyes list 

Creating basslines with the Machinedrum 

For bass lines, I had some good results using some samples that came loaded in the uw upgrade. Also got some results from trx-xt "tom". 

### - mmboom bbb 

I don't have my Machinedrum around at the moment to exactly tell you but the e12 machines with retrigger work well for me. Take a low tom sound for example, make it retrigger with very short inter- 

7 

val and low-pass filter it. Lfo or param-lock the interval and you're good to! 

### - divi bbb 

The second basskick of tr machines (trx-b2 the last one in the list). You can really get some awesome fat basslines out of that one. Just set the hold parameter to a high value, then f*(k about with pitch and ramp parameters. Then set noise, dirt and dist to your liking. Set up a lfo and link that to (for example) filter freq. 

-bartbral bbb 

### Bass drum tips 

Here is one of the many methods that I use to destroy a speaker with the Machinedrum's kick: 

1 - Go into the master effects section and do a teeny tiny little boost on the low end. 

2 - Then, go to the one labeled "p" and pull it down a bit. Make a medium width q, and sweep it down near the low end to about 200-250hz. Listen to how it clears out headroom for the low frequencies! 

3 - Go into the dynamix part and turn the output gain up while lowering the comp threshold. 

### -milkmansnd bbb 

Compression on individual outputs is what gives me punch; or try the dynamix with short attack, long release, hard knee, hp set so that the kick does not over trigger the compressor, threshold and ratio to taste. 

### -meca bbb 

1. First problem was that I was trying to do beats that sound somewhat like what you hear on the records. I was trying to get dynamics of the beat match the finished records. This was wrong from me. I've find out, that to make good beat, it has to have pretty much more dynamics than what you hear with common records. Kick and the snare needs to have a lot of volume, a lot more than others. Then when you add some basslines, melodies, harmonies (if you do any of those), the kick and snare will settle to be right amount punchy. If you don't have enough dynamics, kick and snare will drown under other sounds; you lose the punch. 

2. Machinedrum gives me a starting point, different from using samples. Usually I put some reverb to whole drump attern (rounds the kick) and one one-band compressor after it; makes reverb pump a bit and enlivens the drums. What I'm trying to say, that working with Machinedrum is a whole drum production route (with it's compressors and reverbs and stuff you might add), unlike using samples (where you can do useful stuff by just making the balance right (other people have done the other work for you)). It is a longer route, but at the end the day when you have mastered it, you can make pretty much anything work. 

3. Using trx-kick I used to be in this mindset, that trx-kick needs to be sound right with it's machine parameters. I tend to think it's odd to use for example lpf for it, because the originals didn't have it. So using the trx-kick with the way I think the original tr-machines were used, I should get good results. This was also wrong thinking of me. Trx-kick will sound a lot more analogue, if you use a lpf on it a bit. Just get the dynamics right and it will work (see number 1). You can experiment a lot, just get the dynamics right in the end. 

4. To get good bass end to song does have a lots more than a kick (or even bass). What I mean to say, stuff that sounds shit on it's own, can and will sound terrific with right material glued on top of 

8 

it. Then how to find what works and what doesn't. I just try different things (mainly from mnm), till I get the right one. Then I program it to fine tune it. If I try to program stuff to sound good from the scratch, it tends to sound too weak at the end of the day (night that is). 

-toni bbb 

One way to add some beef to your kick might be to add a GND sin machine and chain it to your kick. 

### - niall munnelly list 

### Snare drum tips 

I've made snares with e12-snare-machine a lot. You can also find some kind of brushed snare there (with high distortion I've managed to get it to decent snare sound). Another good snare is Trx-snare, which is so close to tr808 snare! But this is what you probably know already. Here is some things how I do my snares: 

1. I always try set the pitch of snare very carefully. Sometimes when you have 'good snare sound' you find out later that it's too whimpy, because the pitch is set too high. As for me, I like sizzling snare and it happens a lot of times. I always try to watch that the fundamental hit is somewhere 400 Hz. If it is higher I know it's going to be problem later on. 

2. I have also found out, that I cannot hear if snare sound is good or bad, until I play it with other material. You can get very high-end sounding snare with e12 and pitch set only to '3' and little srr (8), when you have right background. This is true for trx-snare also, sometimes it doesn't sound like 808-snare at all when in right background. 

3. Sometimes I layer snares. Like for now I'm doing new song and I have trx-snare layered with Pisnare. I love the way it sounds! Must be the best snare sounds I got from Machinedrum! 

4. I like to do fills and rolls with snares a lot. Because of this I almost always do one snare that is the accented one. It might be layered. So I hit my accents with this. Then I usually use e12-snare with retrigs to do rolls and stuff like that at lower velocities. Getting a good rolls is really a matter of velocities and I feel it doesn't matter what's the basic sound on those rolls, just make it soft enough and they start to work. 

### - tahvenaine2002 list 

I also find in the stereo outs I should pan different parts in different areas. This is also good with the stereo delay. I pan it like a real drum kit is set up. After I pan everything I use the filters in the routing? menu. Not the synth menu. This puts the drums in a high or low part in the pan. The pan is left and right, and the filter is up and down. 

I think the filter will really put the snare out there. Not to mention using the multiple trigs. I haven't done this yet. But once you get one snare set in the stereo pan copy that machine and make another one in the same pan and everything but change the synth parameters. I haven't done this yet. I've just been thinking about it. 

### - endlessnessisticman list 

I've never tried this. but my Machinedrum snare tip is to copy the track trigs to another track and then change that track's machine to a tom sound. then turn it down very low with a medium amount of decay. you shouldn't be able to tell that it's a tom sound, 

but it should thicken up your snare. If you take a real physical snare drum and turn the snares off, it sounds like a high-pitched tom; you can hear the body of the drum. once the snares are on, they 

9 

create a noise effect and blend with this body sound; that's what makes a real drum sound thick/full. so when your Machinedrum snare has too much noise and not enough body, adding this tom sound on another track should do the trick. you just need to be careful to balance the levels right so that it doesn't sound like a tom but like they are the same instrument. 

You can also use the multiple trigging option for this, but I don't since I usually have some quiet snare "ghost" notes that I don't want to add this sound to. 

### - Joseph Melnyk list 

### Mimicing snare drum 'flam' sounds: 

You can also use the retrig function on the e12 sounds. 

### - belplascaprod list 

### Making snare rolls: 

Check out re-trigger with the e12 drums. It really speeds up production time for rolls. If you use parameter slides over a roll that starts off with a low volume, then builds to full volume, it sounds wicked. Also play around with sliding other parameters during rolls. One of my favorites is setting fltw low, then sliding fltf from a low setting to a high setting. 

### – sidstationownsyou list 

I've been messing with a e12 rim shot with hi retrig with slow timing. Some other tweaks too, and some echo too. Well I go to the filter and give it a high q. Then I turn the filtf or filtw and the sound goes from brain-rumbling to 303 style squeeks. 

### – endlessnessisticman list 

### Making 909 style hihats 

I was able to create similar cymbal sounds. Try experimenting with the ride cymbal machines (efm and e12). The Machinedrum lacks such good samples, but it's a synth, not a sampler! Don't worry, the Machinedrum cymbals are fine and "fresh" sounding, you just need to work a bit on the parameters. 

### - federico ciapi list 

I find that the hats need bit reduction (only a little) and a huge peak with the Eq and sometimes the res filter too. Seems to bring out the bite and sizzle present in the original 909 samples. 

### – amosluyk list 

Bit reduction sounds like a good idea since the original 909 samples for the hats are something like 6 bit IIRC. Also, if you want to emulate a 909 fully you should only tweak the controls available on the original machine during playback or it won't sound genuine. That is, once you've made a 909 hatsound don't touch anything but the decay. 

### - Niklas Winde list 

10 

Crunchy drum sounds 

TRX -> SN PTCH DEC BUMP BENV 13 110 44 0 SNAP TONE TUNE CLIP 60 127 127 57 AMD AMF EQF EOG 0 0 24 63 FLTF FLTW FLTQ SRR 39 49 109 11 

DIST VOL PAN DEL 127 127 6 0 REV LFOS LFOD LFOM 1 108 66 64 

Put the LFO on Ptch, triangle shape plus reverse exp env Free-running, SPEED = 108 depth=68 Schmix=64 

PTCH=13 = F#4 = 185Hz 

- Crackpot list 

Making tuned sounds in the E12 kit 

E12->RS PTCH DEC HP RATL -37 127 1 127 STRT = anything ( e.g. 41) RTRG = 44 RTIM = 45 BEND = anything (e.g. -16) 

Also interesting is LFO on STRT times! 

### - Crackpot list 

### Echo and Compressor interactions 

Try making a short, sparse sound with a lot of echo at say 105 BPM and echo delay of 16/128 or 24/128 or so. Then, employ the compressor heavily. Move your kick to a separate output, so it doesn't affect the compressor. You can actually hear the echoes get quieter and quieter, than louder and louder, then disappear. I was doing this with the infamous EFM Cowbell. It's really weird, especially in stereo.... 

– Crackpot list 

11 

### LFO Speed Settings 

As far as I know (someone posted here long time ago) LFO speeds 0-63 are synchronized to the tempo of the Machinedrum. 64-127 are faster, with no tempo sync. Speeds of 1/2/4/8/16/32/64 are the one who are equivalent to bars/beats/16ths and such (I do not find the right words here, my excuse is being Swedish :). 

### - Janne G:son Berg list 

### Emulating synth envelopes 

You can use the right lfo's upward ramp set to "trig", with Shapemix all the way to the right, and assign it to volume to get a Slower attack, but it's still not the same as a dedicated adsr. 

### - falabala66 list 

### Using the Machinedrum mainly for melodic material 

I use the Machinedrum for melodic material, although I don't limit it to that. You may want to get the uw upgrade for sampling synth stuff to push it more in that direction. 

As for the limitations of the sequencer (which is the same in the Machinedrum and the uw), the song mode is essential imo. It allows you to chop up your patterns in all kinds of ways. Still, even then there are some limitations that are difficult to get around. Odd time signatures and the like will test your ingenuity. 

### - milkmansnd bbb 

I find the uw much more suitable for melody 'cause: 

-you can use samples with a slow attack (this will be linked to the pitch so the lower you go the slower the attack, higher=shorter) 

-you always have a hold parameter! (parameter lockable sustain) This can be up to 2 bars long. 

-if you sample a note at c3, set pitch on mduw to: 

-12 for ... 

-9 for a2 

-6 for a#2 

-3 for b2 0 for c3 3 for c#3 6 for d3 9 for ... 

-you can loop your samples. 

-you can use a sample editor on your computer to put different "wavetable osc's" samples all in one sample. Say sine-triangle-square-'noise'-pulse-.... 

12 

Then on the mduw use start point to choose an 'osc' type. Put retrigger on full and use retrigger time for basic pitch. Then use pitch parameter to parameter lock your melody. Or use pitch for basic pitch and retrig time for parameter locking the melody. What ever you like :) 

-you can use the ram record/play machines to create real nice arpeggiator type fx. 

### -bartbral bbb 

I love using the e12 hats...with a very tight long retrigger...for buzzing synth leads. Outside of that... I tend to use mine for drums only after many attempts to make complete songs using just the Machinedrum. I just don't have the skills yet. 

### - jsrockit bbb 

### Emulating string sounds with the Machinedrum 

Get an impulse , send it to an external output such as C , connect the output directly into an input machine , give both pulse and input machine delay (short 1 till 20 will work) now you got a physical modelled string sound, based on the karlpluss string algorithm, good luck! 

### - ecz2003 list 

I already tried using the gnd-sn a lot, but I always end up with kind of "bell"-type fm sounds, or some weird "rocket scientist" elektro tones. The Sarpsborg Tune on the Elektron website was made by nils odegaard.. It amazing how those strings sound, and I would really like to recreate them. A long time ago he stated in this forum, that the efm set is the key to this, but when I use it, I just get bell type fm sounds, but not string like sounds. 

### - steril707 list 

Use the lfo's for adding more string like envelopes. Go for the reverse lfo's, apply them to filter width and such. Use the eq to emulate the resonant frequencies of the violin body. 

### - Janne G'son Berg list 

### EFM-HH as a violin type sound 

Pitch: 70 Decay: 96 Trem: 101 Tfrq: 64 Mod: 96 Mfrg: 96 Mdec: 127 Fb: 19 (very important parameter, vary this to get more life from the sound) 

Cut the bass with the filter (Fltf: 64) and the highest treble as well (Fltw: 71). 

Dist: 64 Rev: 127 (with suitable reverb settings) 

LFO-page: Param: Fltw 

13 

Shp2: Inverted ramp Update: Trig Speed: 34 Depth: 54 Shmix: 127 

Parameters not mentioned should be at their original settings for the EFM-HH machine. 

- Janne G:son Berg list 

I used a similar setup. The main differences between our setups are: 

- LFO's; I combined both ramps to get a slow attack 

- Mod/tremolo values; these were both at their presets 

- AMD/AMF; I used these to get a bit more depth out of the sound 

Other than that, they're pretty similar settings. I'm very anxious to try your values and combine them with my values.... I'm sure I'll get something interesting; knowing this machine, that is. 

### - Joe list 

Hint1: The EFM HH machine is very melodic. 

Hint2: Using the following LFO settings will give you a softer attack: shp1 and shp2 set to their fifth waveform, mix set to 100-ish and speed set around 8 or 16. This got me a decent string sound. 

### - Joe list 

### Saving individual drums 

Since there isn't any availability to save individual drums I use to very last kits (60-64) as a favorite kits. for example my kit number 64 is called "fav bd/sd". I save my precious and best working bd's and sd's to that kit (8 bd and 8 sd). Then when I'm in a mood of trying to find good bd for some pattern I might copy a machine from favorite kits and see if they work. It's good also, because I have only 8 fav BD's, so when I create a new and good sounding BD I must compare it to fav BD's and maybe erase some of them. That way my Fav kit evolve over time (at least I hope so). 

### - Toni Ahvenainen list 

### Using effects from more than one track 

Try using the "THROUGH" effect. That way you can route the output of one track into another, and use the track effects of the second one to add distortion to the first one, after the first filter! 

### - Daniel @ Elektron 

14 

## Performance Tips 

### Using MIDI Program changes 

It's a very clever system because you can also send program changes which will only happen once until another is encountered. This way your connected gear won't reset the sound parameters every loop due to the program change. 

### - Ronald list 

### Using the RAM Machines 

Just trig a ram-record machine at the point where you want it to record. Param-lock the length. Trim it later by adjusting the start/end of the corresponding ram-play machine. You don't have to push the record button to record samples (but you do have to record the trigs in the first place!) 

So if you have a ram record in your pattern, it will re-record every time the pattern loops, unless you mute the record track. I usually have the ram record in one pattern, and the ram play in a variation pattern, although all kinds of strange, and I mean strange, feedback stuff can happen when you mix and match them both in the same pattern. 

### - ggoodwin bbb 

I'm still trying to get my head around how to use the ram machines properly as it isn't very intuitive to me. The mduw quick start guide that came with the upgrade has good info but lacks information on the ram-record machines a bit in terms of actual usage or work flow. 

I understand the fact that the ram-record machine is used for doing the actual live sampling either from the mains or the external inputs and that the ram-playback machines are used to play back and trig the samples. I've assigned each machine to its own track in my kit. But how do I enable the ramrecord machine to start/stop recording at particular time and length? When putting trigs on the ramrecord machine track, what does that do exactly to the incoming signal? How are people using the cue parameters? Can you somehow preview what you just sampled before sending out to the main mix? 

If anyone can either point me to a thread or perhaps could type up a quick step-by-step on using the ram machines, I would be very grateful and others may benefit as well. 

### - crixmadine bbb 

I had a pattern with some pretty convincing turntablist-style sounds going on. Used the ram record to sample the mains, then p-locked the hell out of the play machine to create a baby scratch buffa buffa type of sound...it was a bit gimmicky but the principle is cool. Theoretically you could create everything from babies and chirps all the way through flares and tears, but it would be kind of a pain. 

The details escape me at the moment, but I remember getting some ridiculous feedback type of stuff using the ram machines as well. 

Also, I have a concept which I haven't actually tried yet so I'm not sure how well it will work, but the basic idea is to make a pattern based completely on a ram play machine, without making any assumptions about what the actual sample in that machine will be...then other patterns can load up the memory with some samples, either internal or external, and then switch to the first pattern, which will then become sort of like an abstract fill tool. Of course you could use multiple tracks with the same ram play machine and each with different filter/fx settings, to make an abstract drum kit whose actual sound depends on whatever sample you feed to it. 

15 

Now that I have my uw fully patched up, I'm planning some tripped out stuff where one output will go to an fx unit, then into one of the ins to be sampled, which will go out a different output into the evolver, then back into the other ram machine...etc. 

Edit: before I forget, there is one more way to use the ram record that you didn't mention...you can manually trig it and it will record on the spot. You never have to enter record mode at all, if you don't want to. This is kind of cool because you can easily record custom fills on the fly, etc. 

### - ggoodwin bbb 

Haven't explored ram machines much yet, here is one way of using them. 

1. Set up 1 rec-machine to record output. Sequence it with parameter locks. 

2. Now fill up rest of the kit with _multiple_ play-machines. Record something and play with the play-machines. Pitch up/down some of them, use delay, make interesting tones and percussion of them. 

3. Now take mnm-sequence or similar and hit play on both machines at the same time. You can find out endless tones and such; just pattern on the mnm. By the way this works very nicely when recording gameboy's nanoloop software, which is set to different tempo (like 100 on Machinedrum and 75 on gameboy). 

### - toni bbb 

You need to have ilev turned up in order to record the input. So you don't want it around 0 like you settings indicate. And turn ibal up if you don't want the input to be in mono. 

### - chiasticon bbb 

Place a step on ramrec track then turn up ilev (volume to be recorded), if you wanna hear what's coming in to the input turn up cue1 on the ramrec machine. Now, if you wanna hear what you just recorded only, turn down cue1 and turn up cue2 . Go to ramplay track put in a step and you should be able to hear what you recorded and tweak it to another dimension 

-neonleg bbb 

### More on UW ROM features 

The SPS-1UW has 32 different sample slots where you can place a sample.. the sample length isn't important.. however the UW does have its limits, the combined size of all the samples put together has to be smaller then 2048KB. 

there are tricks of putting more then 32 different samples into a UW.. by making a for example 10 second sample of 10 bassdrums and selecting the specific bassdrum by the start time and end time parameter. 

### - Wisefire bbb 

Technicly repeat is like sync. If you want to loop, set it up in your sample before transferring it! Several of the presets use loop points. You could loop the whole sample, or have an one shot attack and looping end (the latter is mainly how we looped the presets). 

### - Daniel, Elektron 

16 

### Machinedrum with DJ decks 

Sounds like you're after the ram machines. These capture live audio and allow you to resequence/tweak/effect/filter/chop/pitch it. You can sample from either of the audio inputs (or a mix of the two, but only in mono), or resample the Machinedrum's output. For DJ work I imagine you'd be more interested in sampling the inputs. The way that recording works with the ram record machines may be a little different from what you're used to, and by that, I mean completely ass-different. You will set up one pattern with a rec machine trigger (set length param to the entire pattern). Make sure the Machinedrum is synced to tempo at this point. Let the record pattern play, each loop it will capture one measure worth of sound. When you record the measure that you want, switch to your ram play pattern (which you set up beforehand). Of course, this pattern will sound totally different depending on what you fed the ram record with. Make sure you use lots of parameter locks with the ram play machine... you can use it as a simple phrase sampler, I guess, but that would be a grave injustice. 

Edit: to clarify: the ram record machine will sample audio whenever it is triggered in a pattern...the Machinedrum itself doesn't have to be in record mode for this to work. You can even live-trigger the ram machine if you want (I think). 

### - ggoodwin bbb 

### General live playing tips 

I cheat when I transition from one song to the next. I create the transition period using an external effects processor. I used to do it with my virus c, but now I accomplish the same thing with my Electrix mofx. Typically, I start into a delay of the Machinedrum's output, and then up the 'regen' parameter on the mofx until the delay becomes a loop. From that point, I'll fuck around with the effects while loading up the next 'song' (I sequence my Machinedrum manually) and either fade the loop out into the next track or kill the loop abruptly--right as the next 'song' starts. I'll soon be getting an external analog filter which I'll also use to these sorts of ends. I think that combining this strategy along with some transitional patterns would be a great way of getting between tracks. -cchocjr bbb 

Was trying something out where I would mute some tracks before the transition. Switch the pattern then unmute using the arm function. I was also trying to get the arming and pattern change really close and quick. The Machinedrum reacted strangely sometimes though since I had to hold function while changing patterns. 

I also did a song with 16 patterns. Every four went together so I'd trigger them in a row live. One kit would be assigned to every four. I started the first four. Tweaked that kit to make parts sound different but not too much. Saved that for the next four, and so on. I could go through each set of four and then even go back to like the second four. The first and third four were breaks and the second and fourth four were 4/4 beats. 

### - udenjoe bbb 

For my first live set with the Machinedrum, I focused on getting the biggest sounding patterns. I did not care about which sound was on which track. I used pattern mode and mutes to build each pattern up and down, with the bass drum on track 1 being the lone transitional element, also I muttered and shouted into the mic to shroud the transition. Not very sonically attractive, but it worked. Another bad point was that I was sometimes left guessing which percussive elements were under each muted track. 

For my second live set with the Machinedrum, I focused more on organization and how I was going to work it live. I kept the bass drum on track 1, snare on track 2, etc, somewhat following the labeling on the Machinedrum panel. Tracks 13-16 were always MIDI machines, each triggering a certain type of sample in each pattern. So the execution on the Machinedrum level was in control, but I think the 

17 

overall impact of the patterns were compromised and didn't have nearly the energy of the first set, because I stuck to my pre-fabricated layout for every pattern (as opposed to having the latitude to do special, impactful things like layering and using contrasting snares and high-hats and creating custom synthetic tones, which don't fit traditional categorization.) 

For my next live set, here's my rationale: in trying to squeeze everything on the Machinedrum, I've discovered that basically I can't do what I want with just the Machinedrum. So I've added (3) more external sequencers: rs7000 for instrumentals and vocal samples, emx-1 for 808/909/other standard sampled percussion and va basses/synths/efx/etc, the esx-1 for custom sampled percussion and other samples, and finally the Machinedrum for synthetic percussion and most importantly the bassdrum. 

The Machinedrum tracks 1-4 will all contain bass drum variations, so I can instantly add and remove dramatic bassdrum fills, tracks 5-8 will be what I'll call "core Machinedrum percussion" as in whatever I want for each pattern, and tracks 9-12 will be predestined for oh, ch, rc, cc, as per Machinedrum panel labeling. Tracks 13-16 will be where it will get interesting. Here I will do one of the following - either setup ctr-al machines, input machines, or MIDI machines. The idea will be to have a few tracks which can be instantly launched to warp the entire Machinedrum sound.. Or maybe the sound of my other boxes. Fun stuff. 

Again, I'll use pattern mode and mutes to build the song structure live, and likewise for the other (3) sequencers. 

-synthy bbb 

### Using the inputs in stereo 

Those are mono inputs. Try using the filter input machine first. The Gate and envelope require more tweaking and you have to make a pattern With the drum keys with these to hear a sound. The filter doesn't Need a pattern programmed. Also make sure alev and gate are all the Way to the right when you start. You can turn this down later. Keep Trying, you'll get it. 

### - endlessnessisticman list 

I wanted to hook my Machinedrum up to reaktor on my laptop via MIDI and then have the audio output go back through the Machinedrum but wasn't sure if the a/b inputs are mixed to a single mono? 

### – Polyhex list 

Hi, you can do exactly what you said, pan the 2 inputs hard left and hard right. 

### – Creepjoint list 

### Using the Machinedrum as an effects unit 

I've found that the easiest way to use it as an effects unit is to use the gate machine. you old need to trigger it once (I.e. you don't have to have a sequence running all the time) to allow sound to come through and then adjust it as needed and utilize the delay/reverb/filter. I do this with my Sidstation all the time (to gate the noise as well as to use the Machinedrum's effects). 

### - Joe list 

18 

## General Machinedrum tips 

Little dots next to Kit names in the Load Kit screen? 

That means that that kit (with a dot) has no pattern assigned , and therefore the Machinedrum shows you wich kits are unused.... 

### - Ed Beukers list 

### Did I see Elvis? 

Today, when I have push the on button, I have seen an easter eggs with Elvis and this sentence "the machinedrum is a nice piece of gear" or Something like this. It was very fast and when I try to turn off, and turn On, the easter egg doesn't appears anymore !!! 

I have send a mail to elektron and the respond "we don't know about that"... With a big smiley at the end of the mail :) 

### - onosendai list 

### Serial Number = Machinedrum number? 

I would like to know if the white sticker on back of my Machinedrum is the serial number. If it is how do I know if the machinedrum is one of the first 200 units with the bad 9 twist knobs. This is the number on the sticker d20400357-45. So would this unit be 357 or 45? 

- fuentes925 

I believe its 357. Mine is 544. 

– endlessnessisticman list 

### Using the Compressor 

With short release times the compression just lets go sooner after the sound to be noticed (not unlike the release in the reverb). The longer the time, the more noticeable the effect after the sound). The compression is right there, but the effect does not carry to the empty spaces. 

What you're trying to achieve with your compressor is increasing the "volume" of your mix without increasing the output levels. What you want to do is add body to your mid-ranges and tighten your bass while bringing down you highs into the mix (I hope this is making some kind of sense, so far). As compression will affect everything in the mix, many recommend applying compression first and later applying eq (where you will bring back some of those highs lost in compression). 

On a related note, I recently took my Machinedrum to a studio to help a friend do some tracks. He's been a sound engineer for years now and commented on how the Machinedrum's compressor felt weird (not bad, just different). I've also felt this sometimes, while trying to compress minimal techno tracks and trying to achieve a sort of warehouse feel. If this is what you're trying to achieve yourself, then let also the reverb be big into the mix (with a lot of low freq, and little hi freq. so that it gets "lost" after you compress it). 

19 

### - sr minimo list 

It took me a while before I got to grips with the compressor. This is what I did: 

1. Put a handclap with looooooong decay on beat 2 and 4. Quite low volume, I'd say about 40/127. 

2. Kick drums on beat 1,2,3 and 4. Max volume. 

3. Go to the compressor, lowest possible value for triggering compression, maximum compression ratio. 

4. Minimum attack time, maximum decay time. 

Start the pattern, lower the decay time until you hear the compressor start pumping. Play around with the rest of the controls for the compressor. This made me understand a bit more of how the compressor in the Machinedrum works. 

### - janne g'son berg list 

To be totally correct, I think the Machinedrum does not have a multi-band compressor, so it is always the whole spectrum that is compressed.. But the side-chain let you filter the signal that will be used to decide the amount of compression to be applied. So if you use only hf, the compressor will not react to the bassdrum and over-compress your track. 

### - m-.-n list 

### Tempo syncing the LFO's on Machinedrum and mnm 

You should have noticed by now that the power-of-two lfo values correspond to tempo subdivisions on the machines. So 2, 4, 8, 16, 32, 64 and 127 (not 128, oddly) correspond to different tempo subdivisions on both machines. On the mnm, this can be multiplied by the mult parameter as well (so with mult at 2, an lfo value of 2 becomes 4, 4 becomes 8, etc.). So long story short... With mult set at 1, the lfo values should be the same on both machines. And if you're using a power of two, they should be tempo-synced in one way or another as well. 

### Chiasticon - bbb 

### Pops and crackles 

I sometimes have little pops coming out of Machinedrum. But I know these are coming by Machinedrum reacting to electricity network. If I turn the light off at my bathroom, I will get a pop. It would be nothing, but it gets annoying because neighbors light-switches also make my Machinedrum pop!!! Just to clear; the nonfunctioning thing here is the electricity network of my house (it's an old house). 

### -toni bbb 

The Machinedrum should not make crackle and pops, only certain cereals. First update to os1.30, then see if the pop goes into the delay. Also do the basic strip-down test - disconnect all cables (MIDI included), use another power outlet, preferably in another house, see if the behavior is the same. If it continues, contact support. 

### - Daniel, Elektron 

The clicks that I have complained about are caused by large and fast changes in the filter cutoff. If the cutoff frequency makes a big jump too quickly [either because of 1) a parameter lock, or 2) an LFO] it will "Pop" (often randomly and unpredictably). Elektron have confirmed that this is the way a filter 

20 

will behave on any instrument and it cannot be corrected. That's just the way a filter is. 

### - machinedrum99 list 

### Cases 

Maybe try an old trumpet case too. I know a used trumpet case fits my virus a tabletop perfectly. And for around $30 u.s. used. Check with whoever sells used orchestra instruments for school bands and Orchestras. Old beat up cases can be cheap but sturdy. 

### - jmundo list 

Any photography store will have nice hard plastic flight cases for around 50-75 bucks. 

### - cryogenick list 

I think I'll save up for a pelican case. They are pricey at around $200, but they are advertised as indestructible and airtight, and I figure that it is worth protecting an instrument like this completely. Their model 1500 is a good size comes in silver plastic that would look nice with the Machinedrum, and it uses that "pick and pluck" foam so that you can get the perfect fit. Camera shops carry these, and www.pelican.com has dimensions and photos. Also check http://www.all-pelican-cases-4-less.com 

### - jvs list 

I got a case I'm really happy with: the pelican 1500. As hard as it is to spend money (around $130-150) on a case rather than more gear, anybody with an Machinedrum knows it's worth protecting. If you're not familiar, pelican cases are advertised as indestructible and are completely air tight (if you change altitude you have to open the purge valve to release the vacuum inside!). The 1500 size gives you plenty of room around the edges of the Machinedrum, and once you pluck out the foam you don't need the machine sits in there nice and snug. My Machinedrum's cardboard box has had it. Since I got a case I've been traveling more with the Machinedrum, never give a second thought about tossing it around. It can be propped up within the case for playing at a better angle and keeping it off yr buddy's bar where someone is waiting to spill on it :) 

### - jason vernon starnes list 

I heard DAP makes a microphone case which matches the Machinedrum exactly. 

### - yvobogers list 

I got my case for $25 from Loewe's Hardware. Home Depot has similar "adjustable" tool cases with foam packing. Groovy. 

### - W. Brent Latta list 

I use a Flightkit! Machinedrum fitting fine and I still got space enough for all the cables and power supply. There some cheaper once mad out of leather, but also the Aluminum, which looking better. 

### - privat_joy list 

I use an Don Skeller SN4 camera bag with some custom cut gray packing foam to transport my machinedrum (and either a nord micro modular or an evolver)... it took about a half hour to cut the foam just right to fit in the bag and hold everything, but it fits the machinedrum and everything else perfectly now. The bottom and sides even have ceramic inserts to avoid puncture damage! The bag itself was 70 bucks or something, and is made locally in Seattle. I'm not sure if they're avail everywhere or just a local thing. 

21 

### - Damon Menne list 

### Replacing the power supply 

The replacement machinedrum power supplies are listed on http://www.elektron.se/products.php?sStatus=mdAccessories page. It is not the most obvious place to look I know! Try placing an order, a lot of companies respond to orders faster than emailed questions-its a fact of life! 

### - Eddie Higginson list 

### Removing the knobs and data encoder 

So, during one of my coding breaks, I finally decided to do it: I took a flathead screwdriver to the knobs of my Machinedrum in order to modify and replace them with more comfortable counterparts.... I have found that it is, in fact, possible to remove the knobs very easily using a flathead: just place the screwdriver flathead right underneath the lip of the knob below where it almost meets the surface of the faceplate, and use a gentle twist motion to pop the knob upward. 

Be aware that a small washer/nut sits inside the knob, to keep its base elevated up and away from the faceplate when it is on the unit, so that friction never is a problem when turning the knob. Don't lose the washer, it will fall out when you remove the knob! You will want to hang onto these after you mod your knobs and want to replace them onto the unit. 

As for the large selector wheel knob, that is also removable - and has a hidden feature underneath: an LCD contrast adjuster! I have been wanting a darker contrast on my Machinedrum since I bought it... and now I am in heaven. 

I highly recommend you use a non-abrasive "padding" that sits "sandwiches" your screwdriver and the faceplate of the unit (and the plastic knob), so you don't scratch or nick/dent the plastics or the surface of the metallic faceplate. 

I will post photos I have taken of my findings, along with more detailed instructions on my website soon at www.soundwithin.net - just wanted to spread the news that it is, in fact, a reality... you can mod the Machinedrum knobs very easily! 

Just be forewarned: I am not sure if this voids any warrantee: make sure you know absolutely what you are doing and if you will still be covered before attempting such a thing yourself.... 

### - Adam Watson list 

Be VERY CAREFUL when removing the big selector knob. Mine really didn't want to come off so I wiggled it back and forth and kinda forced it... and ended up almost ripping a piece off of the mainboard rather than just removing the knob. To fix it I had to re-crimp the connector (not sure what the proper technical term is). I'm pretty certain that if I'd wiggled a bit more it would have been much worse. Anyway, it was a *BAD* situation. Luckily I got it sorted out and no permanent damage was done. I also got my contrast properly adjusted 

### – Mind Mechanic list 

The knob is plastic as we all know. It encoder shaft is the same material. The shaft is about 1/3 moon (like in the sky) shape sticking out about 3/4 inches(1.5 cm?). The knob has a corresponding hole to fit the moon. Just grip and pull slowly straight up, don't pull any force to the side. It's snug 

22 

and smooth. I wouldn't do this too many times. You'll probably wear down the hole and shaft since they are the same material. Slide it back on with one finger smoothly. 

### - endlessnessisticman list 

Have you guys and girls noticed that when you are in classic mode and you a pattern playing that has different kit in extended mode and you switch to extended mode while Machinedrum is playing you get a fabulous fill-in sounds and something unpredictable beauty? Some times it's just modulating noise and sometimes it's cool electronic sounds. I love that feature and it's a shame that you cannot write it in songs. :( 

### - Toni Ahvenainen list 

### Test Mode info 

About the test mode, all of the buttons and encoders should just do some kind of action. They light up leds corresponding to the latches activated by the keys/encoders. And the first four buttons have the special functions of lighting up/turning off all the leds and pixels on the display, just to be able to test them quickly. It's not intended to make so much sense to end users without guidance. 

### - Daniel @ Elektron 

New features that are not in the manual yet 

-- Track copy in extended now also copies the lfo. If the lfo is set to affect a parameter on the same track it is remapped as it is pasted. 

-- If the lfo is set to affect a parameter of another track the destination is kept but the lfo depth is set to zero to avoid confusing results. 

-- Holding function and pressing classic/extended loads the current kit, replicating the same functionality as kit-load-enter. For power users only, use with care, all unsaved changes are lost (still accessible in the undo kit). 

-- When holding stop and pressing play the pattern restarts. 

-- When a mute is pending with function held in mute mode parameter changes for all tracks (inverted digits) is not active. 

-- The lfo are disabled for tracks with ctr-al & ctr-8p machines. Since it cannot be fully controlled. Though you can still use an lfo from another track to control the crt-al & crt-8p machines. 

-- The parameters note, n2, n3, len and vel in the MIDI machines are now controllable by the lfo. 

-- Holding function while turning an encoder relatively changes this parameter for all 16 tracks. Indicated by the number under the encoder being inverted. 

-- Index in bars and beats added to the song window for easier synchronization with other sequencers, such as the mono machine. 

-- If the global-MIDI-base channel is turned below '1-4' it goes to '--', eliminating track-oriented 

23 

send and receive for all MIDI channels. System common and system real-time MIDI messages (such as MIDI timing and MIDI synchronization) are not affected. 

- -- New group in global-MIDI-map editor 'ctrl' for assigning notes to pattern start and stop. 

- -- New table in global-MIDI-map editor 'trig' for controlling the functionality of pattern MIDI trigging. 

-- Indication of the currently selected pattern when holding one of the bank buttons illustrated by a double lock blink. (blink-blink ... ... blink-blink). 

- -- DJ-tempo: pressing left/right arrow key temporarily decreases/increases the tempo by up to 10%. 

- -- Copy/clear/paste added in kit load/save lists. 

- -- Mini-star added after kits that are not used by any pattern. 

- -- Tap tempo algorithm improved. 

-- Song mutes, offset and length are temporary remembered when exiting song mode until stop is pressed or a new pattern is selected. 

-- New machines group, ctr, with machines ctr-al and ctr-8p: 

* ctr-al 

Parameter changes are duplicated as relative changes to all other tracks. Trigs have no direct effect, but can be used to hold locks. Changes are permanent to the destination parameters. The function of ctr-al is the same as holding function while turning, but can be used for adding parameter locks for all tracks. 

### * ctr-8p 

Changes 8 parameters from any track and page controlled from the synthesis page and selected with the effects and routing pages. Trigs have no direct effect, but can be used to hold locks. Please note that changes are permanent to the destination parameters For all ctr machines. 

-- Edit accent, swing and slide either for all tracks or for each separate track. Select all or t:xx in the edit window. 

- -- MIDI program change now changes pattern, not kit. 

- -- Separate selection for tempo and control in and out in the global slot. 

-- The early boot menu is updated to allow for upgrading from one Machinedrum to another. Share the latest OS with your friends, no computer needed, just a MIDI cable. 

------------------------------------------E-N-D--------------------------------------------- 

24 

