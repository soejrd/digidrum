# CHALMERS 



<!-- Start of picture text -->
( oy<br><!-- End of picture text -->

Synthesis of drum and percussion sounds by using frequency modulation 

## Erik Larsson 

Translated from Swedish by Flavien Van Haezevelde with the help of DeepL Translator. 

Thanks to Andreas Henriksson at Elektron for his proofreading and corrections. 

EX048/2000 

Department of Signals and Systems November 2000 

1 

I would like to thank everyone at Elektron ESI AB, who willingly contributed with tips and comments during the work, and to my supervisor at Chalmers; Johannes Wintenby. 

2 

### Abstract/summary 

##### Abstract/summary 

Within this project, a basic FM (frequency modulation) algorithm for the synthesis of drum and percussion sounds was developed. The algorithm is based on the work of J. M. Chowning, who pioneered the use of FM for sound gene- ration. A number of adaptations were made to the algorithm, to make it more suitable for different specific instruments. These include bass drum, snare drum, toms, cowbell, handclap and cymbals. The algorithms were finally implemen- ted in Motorola DSP56303 assembly language. The resulting code will be used in the “Machinedrum''; a drum synthesizer developed by Elektron ESI AB in Goteborg, Sweden. 

3 

### Table of contents 

|**Abstract/summary**|**3**|
|---|---|
|**Table of contents**|**4**|
|**1 Introduction**|**5**|
|1.1 Background|5|
|1.2 The task|5|
|1.3 General information on FM|7|
|**2 FM synthesis, theory**|**9**|
|2.1 The basics|9|
|2.2 Reflected side frequencies, harmonic spectrum|10|
|2.3 Frequency or phase modulation: conceptual confusion|11|
|2.4 Importance of the modulator phase|12|
|2.5 Extensions to the basic concept|13|
|2.6 Decoupled FM|14|
|**3 Construction of algorithms**|**16**|
|3.1 FM Gen|16|
|3.2 The development process|17|
|**4 Results**|**19**|
|4.1 Bass drum|19|
|4.2 Snare drum|20|
|4.3 Toms|22|
|4.4 Handclap|23|
|4.5 Rimshot|25|
|4.6 Cowbell|26|
|4.7 Cymbals (open and closed hi hat, ride, crash)|28|
|**5 Implementation**|**30**|
|5.1 Hardware|30|
|5.2 DSP programming|30|
|5.3 Coldfire programming|30|
|5.4 The noise generator|31|
|5.5 Oscillators|31|
|5.6 The high-pass filter|31|
|**6 Conclusion and commentary**|**33**|
|<br>**References**|**34**|





4 

### 1 Introduction 

#### 1.1 Background 

This thesis was carried out at Elektron ESI AB in Gothenburg. The foundation for Elektron was laid during the project course in digital design at Chalmers Computer science programme, when some Chalmers students came up with the idea of developing a synthesizer around the sound chip that was in the old home computer Commodore 64; SID6581. The result was SidStation, a very odd musical instrument that gives ordinary, "MIDI-oriented" musicians access to the characteristic SID sound. SidStation has received excellent reviews in both the national and international specialized press. After this success, it was time to move on and the design of the next product, the Machinedrum, began. 

Machinedrum can be described as an advanced drum machine or rather a drum synthesizer, since the possibilities to influence the character of the drum sound are much greater than in previous machines. In fact, drum machines are in decline in the year 2000 - hardly any pure drum machines are manufactured today. Some of the sounds of the classic<sup>1</sup> analogue devices live on in the form of samples from various synthesizers and "groove boxes"<sup>2</sup> , but few synthesize their own drum sounds. 

Machinedrum is a DSP-based open architecture, where all sound generation is done in software. This allows the system to incorporate several different synthesis methods and to be complemented by new future versions of the operating system. My task has been to create DSP algorithms for generating drum sounds using FM (frequency modulation), and these are intended to be one of several other sound models in the machine. I have been given very free hands to design the sound algorithms myself - the only requirement has really been that the synthesis should in some way be based on FM. Supervisor at Chalmers has been Johannes Wintenby at the Department of Signals and Systems. Daniel Hansson supervised at Elektron. 

#### 1.2 The task 

The following lines are taken directly from the Elektron job description: 

_The aim of the thesis is to investigate the potential of the frequency modulation (FM) synthesis algorithm for the production of synthetic percussive sounds. The possibilities of the algorithm in this area are so far untested. An adaptation of the algorithm will be necessary in order to produce a desirable character._ 

> 1 Well known and often used sounds come from machines like Roland's TR-808 and TR-909 - now expensive and sought after on the second-hand market. 

> 2 More or less toy-like devices that allow for easy performance of techno music. 

5 

_The work will be based on a scientific foundation requiring a thorough investigation of the characteristics of the algorithm and an in-depth understanding. The proposed applications of the algorithm will be implemented on a DSP (digital signal processor) platform with real-time requirements._ 

_The thesis includes the following steps:_ 

- _The ability of the FM algorithm to produce percussive sounds shall be investigated._ 

- _● The results of this study will form the basis for the implementation of a DSP platform._ 

- _Evaluation of the results and functionality of the work._ 

_Programming is usually done in the C programming language, with final implementation in assembly on the DSP for highest efficiency._ 

_The scale of the task means that this work requires only one person._ 

_Goal :_ 

- _The algorithms used should avoid high calculation complexity as much as possible._ 

- _● It should be possible to generate sounds of the following types: bass drum, snare drum, hi hat, cymbal and toms._ 

- _An important aspect of the generated sounds is that they have an innovative character._ 

- _The program code should be extensively annotated._ 

- _The work should be carefully documented and the difficulties and obstacles encountered during the project should be clearly described._ 

- _In addition, the report should include a section on any other applications of the investigated algorithm._ 

The aim has not been to create lifelike drum imitations, but innovative and characterful sounds where you can hear the type of percussion instrument involved (at least in their standard settings). The following sounds are part of a complete Machinedrum kit: 

- Bass drum 

- Snare drum 

- High tom 

- Middle tom 

- Low tom 

- Handclap 

- Rimshot 

- Cowbell 

- Closed hi hat 

- Open hi hat 

- Ride cymbal 

- Crash cymbal 

6 

The machine has eight knobs for controlling the character of the sound. Each drum model must therefore have at most eight parameters that can be controlled by the user. Determining these controls has also been an important part of the work - the sounds should be easy to control and understand. 

#### 1.3 General information on FM 

Frequency modulation (FM) is a technique used in radio broadcasting. The frequency of an oscillator (the carrier) is allowed to deviate from a fixed mean value. The amount of deviation is controlled by another signal (the modulator) and this signal can then be reproduced in the receiver. Typically, the frequency of the carrier wave is several orders of magnitude greater than that of the modulator (Fig. 1.1). The musical equivalent is called vibrato: slow variations in the frequency of a tone, for example in an opera singer. However, if the frequency of the modulator is of the same order of magnitude as that of the carrier wave (and within audible spectrum), the frequency deviations are too fast to be perceived as a vibrato. Instead, the tone changes character (fig. 1.2); the modulator creates a spectrum of overtones and undertones around the carrier wave - a new waveform emerges. 



_Figure 1.1: Frequency modulated wave_ 



<!-- Start of picture text -->
Modulator carrier Resulting waveform<br><!-- End of picture text -->

_Figure 1.2: FM synthesis creates a new waveform - similar to a square wave._ 

The first to notice this method of synthesizing sound was Dr. John M. Chowning at Stanford, who in 1973 published the first article on the subject [1]. FM sound was very different from what 

7 

had previously been achieved and the FM synthesizers<sup>3</sup> that eventually appeared were groundbreaking at the time. They were not necessarily better - the sound of classical subtractive synthesis<sup>4</sup> , for example, is much more desirable today - but they were more similar to real instruments (although it often takes a bit of good will to hear the resemblance). 

An important part of the FM sound is the "dynamic spectrum" which can easily be created : the amplitude of the modulator controls the distribution of energy in the spectrum and therefore this amplitude is often found to change during the playing of the sound. In this way, a bell sound can start with a complex spectrum (large modulator amplitude) and eventually fade to a pure sine tone (no modulation at all). 

With FM, intricate waveforms can be created by simple means - two sine wave oscillators reach far. The technology has therefore been popular until now in various budget synthesizers, such as those built into video games and simple PC sound cards. Because FM works best with stable, accurate oscillators, most implementations are digital. Chapter 2 explains FM synthesis in more detail. 



<!-- Start of picture text -->
‘ ten pork corral $5 ee ATH Raley.<br>r<br>rn OC figareangWyo waved pape ahdotewry& fyto tlsPOP BaiBET<br>RE enum Sores enepemanenaasoueae ie<br>pi oh lite SURED. 5 sats eslnenkpemsesnte Hansete te<br>| TTT<br><!-- End of picture text -->

_Figure 1.3: Yamaha DX7_ 

> 3 Most notably, Yamaha's DX7 (fig. 1.3), introduced in 1983, definitely colored much of the music of the 1980s. The distinctive electric piano sound produced by the DX7 (designed to mimic the semi-acoustic Fender Rhodes) can be heard in any number of takes, and is still widely used. 

> 4 The most common method of synthesis still today is to start from a waveform with lots of overtones (for example, a square wave, a sawtooth wave or a sample) and then remove frequency components using filters. 

8 

### 2 FM synthesis, theory 

#### 2.1 The basics 

In its simplest form, FM requires two sinusoidal oscillators, or operators as they are usually called in the FM context. One acts as a carrier wave and the other as a modulator. Such a frequency-modulated signal is described by the equation 

𝑥(𝑡) = 𝐴𝑠𝑖𝑛(α𝑡 + 𝐼𝑠𝑖𝑛β𝑡) 



Where 

   - the instantaneous amplitude of the modulated carrier wave 

- 𝑥 = 

- 𝐴 = the maximum amplitude of the modulated carrier wave 

   - angular frequency of the carrier in rad/s 

- α = 

- β = angular frequency of the modulator in rad/s 

   - modulation index 

- 𝐼 = 

   - time 

- 𝑡 = 

When I=0, no frequency deviation exists and no modulation occurs. However, when I is greater (or less) than zero, new frequency components arise, symmetrically distributed above and below the bar wave. These lie with an interval that directly corresponds to the modulator frequency. As I increases, more side frequencies arise and the energy is increasingly moved away from the carrier wave (Fig. 2.1). 



<!-- Start of picture text -->
1=0 ie1+2<br>0 a 0 ft | at arpLt.<br>f=! l=4<br>0Part).» Ptr. wer,<br>a otp 0 a arp<br><!-- End of picture text -->

_Figure 2.1: Bandwidth versus modulation index (I)_ 

It can be shown [2] that when 𝑥(𝑡) is expressed in terms of a Fourier series, the coefficients are determined by Bessel functions , 𝐽𝑛(𝐼) , in the following way: 



<!-- Start of picture text -->
x4) = 5 J(D)sin( cr + nBr +n ~ nl5)<br><!-- End of picture text -->



9 

###### Developing the above expression gives 



<!-- Start of picture text -->
x(t) = A{Jg(Dsinat<br>+J,(J)sin(at+ Bt) - J, (J)sin(at—Br)<br>+ J(1)sin(at + 2B) + Jp(1) sin (at—-2Br)<br>+J3(1)sin(at+ 3Br) —J3(/) sin(at-3B2)... }<br><!-- End of picture text -->

(2.3) 

It can be seen from equation 2.3 that some frequency components have negative coefficients. For some combinations of n and I the bessel functions are also negative. This corresponds to a 180 degree phase shift and should not really play any major role - due to the lack of ability of the ear to distinguish phase differences. In the way FM is used in the context of sound, however, it becomes important: a modulator of the same order of magnitude as the carrier wave often creates side frequencies that are reflected around the 0 Hz axis and then interfere with positive components. 

#### 2.2 Reflected side frequencies, harmonic spectrum 

At certain combinations of 𝐼α , and β , negative frequency components arise (when α −𝑛β < 0 5). When reflected around the 0 Hz axis, they mix with the positive frequency components and their phase is therefore important - two equal components, for example, completely cancel each other out (Fig. 2.2). When α and β relate to each other as two integers, i.e. when α/β is a rational number, a harmonic spectrum<sup>6</sup> is created. Since this is pleasing to the ear, it is often sought after when synthesizing sounds with FM. This is also the prerequisite for negative (reflected) frequency components to interfere with positive ones. Note that the fundamental need not necessarily be emitted by the carrier wave - a side frequency, reflected or not, may very well take over this role. 

The phenomenon of reflected side frequencies has an important effect on the timbre of the sound - often all frequencies below the bar wave are negative (when β > α ) and are thus reflected. 

> 5 Theoretically, of course, such frequency components are always present, but at small I the amplitude of the Bessel functions with increasing n soon becomes negligibly small. 

> 6 In a harmonic spectrum, the harmonics have frequencies that are even multiples of the fundamental. 

10 



<!-- Start of picture text -->
1.Spectrum with phase information<br>2. Negative components projected<br>onto positive ones<br>3. resulting spectrum,<br>without phase information<br><!-- End of picture text -->



<!-- Start of picture text -->
0<br>4<br>'<br>'|<br>:<br>0<br><!-- End of picture text -->

Figure 2.2: Negative and positive frequencies together form the spectrum (3). 

#### 2.3 Frequency or phase modulation: conceptual confusion 

In the remainder of this report, the term frequency modulation is used to mean the instantaneous change in the angular frequency of an oscillator by means of a modulator function of some kind. A closer look at the FM equation (2.1) shows that ⍺ is always constant while 𝐼𝑠𝑖𝑛β𝑡 adds a periodically varying phase shift to the expression. However, this is almost the same as frequency modulation (according to our definition) provided that the modulator is sinusoidal, as we will soon see. If we derive the argument in equation 2.1, we get the instantaneous angular frequency of the carrier wave: 



<!-- Start of picture text -->
d .<br>tata + fsin Be] _ = a+/BcosBr .<br><!-- End of picture text -->



We see here that equation 2.1 with a sinusoidal modulator is actually equivalent to frequency modulation with a cosine, and it does not give quite the same result (see chapter 2.4). In practical implementation of FM, oscillators in the form of sine tables are often used. It is then easy to achieve FM where you really control the instantaneous frequency, i.e. the step length in 

11 

the table. To get the expected result, the modulator must consist of a cosine function, i.e. be phase shifted +90 degrees relative to the carrier. 

In Chowning's 1973 article, the FM formula (2.1) is used side by side with simple "wiring diagrams" that give the impression that it is really the instantaneous frequency that is modulated, without any mention of this important difference. The same applies to a number of subsequent articles. This has led to some confusion over the years - implementations that have been assumed to be equivalent have produced different results. It was not until 1992 that an attempt was made to clarify the concepts and create an "FM standard" (F. Holm, "Understanding FM Implementations: A Call for Common Standards" [3]). 

Frequency modulation according to our definition is described by the following equation: 



<!-- Start of picture text -->
x(t) = sin(ct+ 1[m(B2))<br><!-- End of picture text -->



where 𝑚(β𝑡) is the modulator function. If we derive the argument, we see that this is true: 



<!-- Start of picture text -->
£ [au + I[m(B2)] = 0+ IBm(Br)<br><!-- End of picture text -->



Thus, if we frequency modulate with 𝑚(𝑡) = 𝑐𝑜𝑠 𝑡 , we have achieved the same result as Chowning did with the basic formula (2.1). By phase shifting the modulator, the two methods have been made equivalent, but it is important to remember that this only works when the modulator is sinusoidal - if 𝑚(𝑡) contains a DC component, for example, the entire fundamental is shifted. It may be worth noting that FM, by our definition, with a cosine modulator does not give _exactly_ the expected result from equation 2.1 when implemented digitally - this is because the step through the table oscillator only approximates an integral. 

#### 2.4 Importance of the modulator phase 

We have seen above that there are two main ways to implement FM - according to Chowning's formula (2.1), or the more intuitive solution of "oscillators", where the instantaneous frequency is modulated. Both are widely used in different contexts. We have also seen that these can be made equivalent by adjusting the phase relationship between the carrier wave and the modulator, but have not yet seen the effect of this phase parameter. It will turn out that the sound is affected in a not insignificant way, and it is therefore useful to add a phase parameter, Φ𝑚 , to equation 2.2. This gives the following, more general, equation: 

12 



<!-- Start of picture text -->
x(t) = Asin(at—Icos(Br+©®,,))<br><!-- End of picture text -->



The Fourier series then becomes [2]: 



<!-- Start of picture text -->
xt)= > Jyy(Dsin( a + nBr-+ no, ~ nl)<br><!-- End of picture text -->



Insertion of Θ𝑚 = π/2 in 2.7 and 2.8 gives equations 2.3 and 2.4 respectively. It can be seen here that the modulator phase does not affect the sideband amplitudes in any way, and this is probably the reason why the phase parameter was previously ignored. The sidebands phase is affected to the highest degree. This is, as we have seen, important because of the phenomenon of reflected sidebands: when a negative, reflected, frequency component coincides with a positive one, the phase determines the amplitude of the resulting component. The above reasoning also offers a new way of explaining a valid phenomenon in FM synthesis: when the conditions for interference are met and the waveform and modulator are adjusted to an almost harmonic spectrum, slow cyclic variations of the tone's timbre can be heard. The conventional, and quite correct, way of explaining this is that pairs of adjacent frequency components are formed. These interfere with each other and low-frequency oscillations arise. Instead, one can imagine that a slightly off-axis modulator corresponds to an on-axis modulator with a continuously linearly varying modulator phase, thus causing cyclic variations of the timbre. 

These two explanations are understood to be mathematically equivalent. 

#### 2.5 Extensions to the basic concept 

The basic case of FM that has been described can be extended; a rather obvious extension is to use two modulators (Schottstaedt's "Complex Modulating Wave" [4]). Advanced FM synthesizers such as Yamaha's DX series have more operators; the DX7, for example, has six. These can then be combined in various pre-established configurations. There are also feedback loops inserted in the signal path (see chapter 2.6). On some synthesizers you can also use other waveforms than sine. 

Among the more odd variants is asymmetric FM, AFM [5], where the basic formula is modified so that its Fourier series is instead 

13 



<!-- Start of picture text -->
x(t) = YF r'In(Dsin(at+ nBr)<br><!-- End of picture text -->



There is an additional factor r that creates asymmetry in the spectrum. Note that this is different from using a filter to create asymmetry, as r has the same effect whether the frequency component is reflected or not. However, this method has not been widely adopted, perhaps because it is relatively difficult to calculate (see [6]). 

Another variant that also produces an asymmetric spectrum, but requires much less processing power, is DFM (Double Frequency Modulation) [7]: 



<!-- Start of picture text -->
x(t) = sin(J,sino,+ I,sina, t )<br><!-- End of picture text -->



DFM, which is in fact a special case of Schottstaedt's "Complex Modulating Wave", has not gained much importance either. 

It is often difficult to find good parameters for an FM synthesizer - the possibilities for variation are almost infinite. Some interesting methods for how FM parameters can be "matched" to achieve a certain sound have been developed. A common feature of these methods is the creation of a time-variant spectrum of the sound to be synthesized. Usually only harmonic overtones are considered, which makes FFT work well for this analysis (this also makes the methods less suitable for drum sounds, which are usually more complicated than that - especially in the attack). Some then try to systematically fit the FM spectrum to this reference [8], while others use genetic algorithms [9, 10]. The latter seems to give the best results for FM. 

#### 2.6 Decoupled FM 

By phase modulating an oscillator with its own output signal, the waveform can be influenced in an interesting way: as the feedback increases, the output signal shape approaches a saw tooth. When it is increased further, high frequency oscillations occur in the middle of the wave. If you feedback too much, the system behaves chaotically - we have created a noise generator (Fig. 2.3). 

The "pseudo-saw tooth" that can be created with feedback contains a DC component. Therefore, the fundamental frequency of the carrier wave can be shifted by modulating with a feedback oscillator in an FM implementation with "real oscillators". Feedback loops can also be long, so that one oscillator is modulated by another one further down the signal chain - this is for example found on the DX7. 

14 

The method can also be interesting for creating an aliasing-free digital saw tooth. 



<!-- Start of picture text -->
- _AS<br>_<br>_<br>| 1 | | \<br>NI — | ! |<br><!-- End of picture text -->

Figure 2.3: Sinusoidal oscillator with feedback: appearance of the waveform at different degrees of feedback. 

15 

### 3 Construction of algorithms 

#### 3.1 FM Gen 

Synthesis of drum sounds using FM is a relatively unexplored area (a little can be found in Chowning's classic article[1], p.533).A natural first step was therefore to provide a platform where different algorithms, known and unknown , could be easily tested. Instead of using any of the existing programs for such applications (e.g. CSound) or any mathematical tool (Matlab, Mathematica...), a special program was written in Visual Basic; FM Gen (fig. 3.1). A proprietary software gives full control over how all calculations are performed, which made it easier when the algorithms were finally implemented in DSP assembly. It is also more straightforward and flexible, and you can also be sure that the sounds you find can indeed be reproduced exactly in the DSP. 

FM Gen acts as a "framework" where the sound generating code can be easily modified and replaced. The program provides a number of "sliders" that are linked to key parameters in the algorithms. By pressing the "Play" button, the sound is calculated in a buffer and then played back. It is also possible to save interesting settings that you find. 



<!-- Start of picture text -->
erate ; Whe) a: Eifel 3<br>WH RT<br>SUA hha 2h af SUSY py5 FRR PERTRP Teongipar DTTC LE2SGreaTY<br>aesfa ee EY ‘3. eam! SSE ef) 49) bastrekustrock par) | ————~ 2.<br>aa | a ae oar 25S)" |bastrummaciup & ty ae ie:<br>BGBed had ReCTbp a Reae591) bastrummael perume ram pjone—5| if ghar<br>=O ees ea Ot ek a eS: Co eeeid bastrumme’siunc ome eis ene<br>er aad Nae BC es: Bistad eee cent la astrumma sjungon «lisse ie<br>mei 5 + De es aa wad: fo AD BRre ® bra crash.parl i Gt aebi<br>fe elMe fat al PeA Fa edak A al}ReI eka: easBe at he eeeacmeaap ts|oracymb| Ibracymb2.parbe cymb 3.perlpart |.aepeeARSa  aeMEH |<br>Raion eae OSTEO — Sanaa Hl brus kortare attack;’} : casi<br><!-- End of picture text -->

Figure 3.1 : FM Gen 

an example of what the simplest possible FM generator might look like in VB: 

```
ForI&=1ToNSamples
Wavedate!(I&)=SinOsc1(CarrierFreq!+ModulatorIndex!
*SinOsc2(ModulatorFreq!))
Next
```

With the functions and variables provided by FM Gen, it is easy to try different algorithms and the program proved to be a very valuable tool in the search for interesting sounds. 

16 

#### 3.2 The development process 

The simple 2-operator FM model described by Chowning [1] was chosen as a starting point (Fig. 3.2). This was modified and extended in various ways. Some variants turned out to be unusable in this context, while others could actually create drum-like sounds. Attempts were also made with DFM and AFM, but none of these gave particularly useful results. 

Chowning's model contains 2 operators; a modulator and a carrier. It also includes two envelope generators. An envelope generator creates a signal that varies with time in a certain way; the envelope. This signal can for example be used to control the amplitude of an operator. Envelopes can look different depending on what they are used for - the volume envelope of a piano sound is a slowly decaying signal, while drum sounds usually have a fast envelope. In the current model there is one envelope for the volume (carrier amplitude) and one for the modulator amplitude. The modulator envelope means that the amount of modulation is varied over time and the "dynamic spectrum" mentioned in the introduction can thus be adjusted. An offset is added to the envelope, so that the sound can "land" at a certain modulation depth. Chowning uses so-called ADSR<sup>7</sup> envelopes, but recommends exponentially decaying volume characteristics for drums. 



<!-- Start of picture text -->
Sn<br>_<br>,—o-<5© ‘<br>oT} *<br>utsignal<br><!-- End of picture text -->

Figure 3.2: Chowning's model (see chapter 2.3 for implementation details). 

After some experimentation, based on the model described above, an extended version was developed that produced some interesting results. This became the new "springboard", and all the algorithms finally developed, with the exception of the cymbal, are based on it (Fig. 3.3). The 

> 7 "Attack, Decay, Sustain, Release" - the most common way to control the shape of an envelope. 

17 

oscillator blocks here symbolize table oscillators (see chapter 2.3), where the instantaneous frequency can be controlled. The frequency of the modulator is linked to that of the carrier, so that the relation between them is constant. The modulation depth (amount of modulation) is also controlled by the carrier frequency - otherwise the modulation decreases as the frequency increases. Feedback loops are present on both the modulator and the carrier. The offset added to the modulator envelope in Figure 3.2 was removed. Finally, there is a third exponentially decaying envelope that controls the fundamental frequency of the sound - useful in the synthesis of bass drums. Originally, there was also a fourth envelope that controlled the modulator frequency. This unusual arrangement produced curious effects, but never made it into any of the final implementations. 

The model described above can, with the right parameters, give surprisingly "acoustic" results. A lot of feedback on the modulator (resulting in noisy tones) and a short modulator envelope can sometimes make the impact of a tom relatively realistic. Combined with the frequency envelope, the result is even better: a short sweep in the beginning makes the attack " snappier ". It can also be useful to let the oscillators begin a short way into the period, thus creating a "click" at the very beginning of the sound. 



<!-- Start of picture text -->
4— O————— i<br>d Ls or? ;<br>Y<br>S o><br>&<br>4,<br>utsignal<br><!-- End of picture text -->

Figure 3.3: Core model 

18 

### 4 Results 

#### 4.1 Bass drum 

This model consists of the core model minus the feedback on the carrier (Fig. 4.1). The knobs on the Machinedrum are linked to the following parameters: 

- Frequency (𝑓𝑏) 

- Decay (volume envelope decay time) (𝑑𝑏) 

- Modulator frequency (𝑓𝑚) 

- Modulation depth (𝐼) 

- Modulator decay (𝑑𝑚) 

- Modulator feedback (𝑏𝑚) 

- Frequency envelope amplitude (𝐴𝑓) 

- Frequency envelope decay (𝑑𝑓) 

For a more distinctive attack, the oscillators are triggered with a phase shift of 𝝅 /2 rad (i.e. cosine). The sharp edge at the beginning of the waveform creates a high-frequency "click" sound similar to the hammer hitting the drumhead. The most realistic sound is achieved when the modulator is feedbacked so that it produces noise and the modulator development is relatively fast. Otherwise, the model can be manipulated endlessly to create all sorts of unusual sounds. 

19 



<!-- Start of picture text -->
4,<br>: &>e—< 4,<br>W/ ! ;<br>\/<br>Sy &<br>&<br>o—IK}-4<br>utsignal<br><!-- End of picture text -->

Figure 4.1: Bass drum 

#### 4.2 Snare drum 

With the standard model, reasonably good snare sounds can be created. Approximately the same settings that produce a good bass drum can be used for the snare, if tuned higher. In order to get a good sound, however, it is necessary to recreate the sound of the wire, the metal band that is stretched against the underside of a snare drum. This device creates a sizzling sound when the drum is struck and is the main reason why the snare differs so much from a regular tom. This model uses a noise generator followed by a high-pass filter - the lower frequencies are not desirable in this context. You can also use a bandpass filter, so that the noise is not too harsh or sharp, but the result was satisfactory with high-pass filtering only. The frequency envelope present in the standard model is removed - the number of parameters has to be limited and it is not so useful in the synthesis of snare sound. Last in the signal chain is another high-pass filter, which is useful if the sound is too dull or " boxy ". The following parameters can be controlled by the user: 

20 

- Frequency (𝑓𝑏) 

- Decay (𝑑𝑏) 

- Modulator frequency (𝑓𝑚) 

- Modulation depth (𝐼) 

- Modulator decay (𝑑𝑚) 

- Noise level (𝐴𝑏𝑟𝑢𝑠) 

- Amplitude of the frequency envelope (𝑑𝑏𝑟𝑢𝑠) 

- Cut-off frequency of the high-pass filter (𝑓ℎ𝑝) 

The modulator feedback (𝑏𝑚) is fixed at a high value (produces a lot of noise). The high-pass filter after the noise generator is also fixed at a fixed value (𝑓𝑏𝑟𝑢𝑠) . 



<!-- Start of picture text -->
h, &» Sn<br>ore 4,<br>i) \/ I<br>><br>Avyus %<br>\/<br>Ip<br>utsignal<br><!-- End of picture text -->

Figure 4.2: Snare drum. 

21 

#### 4.3 Toms 

The same model used for the bass drum (Fig. 4.1) can be used with advantage for toms - all that is required is to adjust the frequency a little higher. However, the sounds have a tendency to become rather "muddy" and muffled. Therefore, a high-pass filter is placed at the end of the signal chain. Its cut-off frequency is linked to that of the fundamental and cannot be controlled in any other way by the user. It is also possible to control the initial phase of the oscillators and thus vary the amount of "click" at the beginning of the sound. Controls: 

- Frequency (𝑓𝑏) 

- Decay (𝑑𝑏) 

- Modulator frequency (𝑓𝑚) 

- Modulation depth (𝐼) 

- Modulator decay (𝑑𝑚) 

- Frequency envelope amplitude (𝐴𝑓) 

- Frequency envelope decay (𝑑𝑓) 

- Starting phase ( variable between 0 and 𝝅 /2) 

The modulation feedback (𝑏𝑚) is fixed to a high value. 

22 



<!-- Start of picture text -->
> In<br>4<br>] Ls >< fhKRf— 4,<br>©<br>cA Wy 1<br>&<br>> 4,<br>utsignal<br><!-- End of picture text -->

Figure 4.3: Tom 

#### 4.4 Handclap 

A clap-like sound can be created with the standard model: the modulator is feedbacked so that it is noisy and the modulation depth is set quite high. This results in something like white noise with a resonant peak, which, combined with a quick volume envelope and a little imagination, produces a clap-like sound. However, what you really want to achieve is the sound of several people clapping at the same time. A common way to recreate this effect is to use a special volume envelope, consisting of a number of rapid "pre-claps" before the final decay. This gives the impression of several simultaneous claps that are slightly "out of sync". The envelope operates in this case so that it retriggers when it reaches 10% of the maximum amplitude - this is repeated until it is time for the last, longer, decay. 

Last in this model is also a high pass filter - the sound can often be too dull otherwise. The following parameters can be adjusted: 

23 

- Frequency (𝑓𝑏) 

- Modulator frequency (𝑓𝑚) 

- Modulation depth (𝐼) 

- Modulator decay (𝑑𝑚) 

- Pre-clap decay (𝑑1) 

- Decay (𝑑2) 

- Number of pre-claps (𝑛) 

- Cut-off frequency for the high-pass filter (𝑓ℎ𝑝) 

The modulation feedback (𝑏𝑚) is fixed at a high value. Figure 4.4 describes the handclap. 



<!-- Start of picture text -->
f, o> Su<br>o> d,<br>/s<br>L<br>©<br>pao!<br>utsignal<br><!-- End of picture text -->

Figure 4.4: Handclap 

24 

#### 4.5 Rimshot 

This sound can be characterized by (at least) two components; the drumstick hitting the edge of the snare drum and the resonance of the drum itself. The former is created in the same way as the clapping sound (see previous section) - the modulation noise is preferably set slightly lower, which gives more tone in the sound. The snare sound is created in the same way as in the snare model, but without the noise generator. In addition, the modulator envelope has been removed. At the end is the usual high-pass filter (fig. 4.5). Controls: 

- Frequency, edge stroke (𝑓𝑏 𝐵) 

- Decay, edge stroke (𝑑𝑏 𝐵) 

- Modulation depth, edge stroke (𝐼𝐵) 

- Frequency, snare (𝑓𝑏 𝐴) 

- Decay, snare (𝑑𝑏 𝐴) 

- Depth of modulation, snare (𝐼𝐴) 

- Snare sound volume (𝐴𝐴) 

- Modulator decay (𝑑𝑚) 

- Cut-off frequency for the high-pass filter (𝑓ℎ𝑝) 

The modulator feedbacks (𝑏𝑚 𝐴, 𝑏𝑚 𝐵 ) are fixed at high values. The relative modulator frequencies (𝑓𝑚 𝐴, 𝑓𝑚 𝐵 ) are also fixed. 

25 



<!-- Start of picture text -->
Ina & Sina Soa > In<br>1<br>\/ \/<br><< ><br>* 4,4 © 4,»<br>&<br>4, W/<br>f,<br>ulsignal<br><!-- End of picture text -->

Figure 4.5: Rimshot 

#### 4.6 Cowbell 

This instrument has a special character - a little false or untuned somehow. Roland's old analog drum machine TR-808 uses two detuned square wave oscillators, and the result is quite pleasing: the sound, which is actually not very similar to the acoustic original, has a very characteristic and pleasing timbre and the question is whether it has exceeded its predecessor in popularity<sup>8</sup> . 

In order to make it simple, the TR-808 was used as a starting point. Instead of square waves, two FM pairs with feedback on the carrier were used - in this way, something similar to the sharp sound of the square wave can be created (see chapter 2.6). In the TR-808, one oscillator has a frequency that is approx. 48% higher than the other, and the two FM pairs were therefore tuned to this ratio. 

> 8 Listen to the intro to Whitney Houston's "I wanna dance with somebody" - you'll probably recognize the sound from many other songs. 

26 

The volume envelopes are done a little differently: one fast and one slow are added and the result is a sharper attack with a longer decay time. The two modulators share the same envelope - two separate ones just give too much control. The same goes for the feedback. 

- Frequency (𝑓𝑏 𝐴, 𝑓𝑏 𝐵 = 𝑓𝑏 𝐴 * 1. 48) 

- Decay (decay time of the volume envelope) (𝑑𝑏 1) 

- Modulator frequency (𝑓𝑚) 

- Modulation depth (𝐼) 

- Modulator decay (𝑑𝑚) 

- Carrier coupling (𝑏𝑚) 

- "Sharpness" (𝐴𝑏 2) 

𝑑𝑏 2 is calculated so that it is always significantly shorter than 𝑑𝑏 1. 𝐴𝑏 1 = 1 −𝐴𝑏 2 . Figure 4.6 describes the algorithm. 



<!-- Start of picture text -->
Ia © Sn & Son<br>> 4,<br>& &<br>;<br>\/ \V/<br>©&<br>4<br>4,7 W<br>4, > [=> &><br>A,<br><!-- End of picture text -->

Figure 4.6: Cowbell 

27 

#### 4.7 Cymbals (open and closed hi hat, ride, crash) 

Trying to synthesize cymbal sounds is a major challenge; the sounds are so complex that they are difficult to replicate with simple "analog" models. They should have a wide spectrum with lots of frequency components but not be too noisy. Once again we looked at the Roland TR-808, where a solution similar to the cowbell is successfully used - in this case six square waves with mutually dissonant tuning. 

Since FM is quite heavy to calculate, six FM pairs, i.e. 12 operators, are completely out of the question in this context in order to meet the real-time requirements (max. approx. 130 machine code instructions/sample). Four pairs with mutual tuning taken from the 808 were enough. To further increase efficiency, "phase modulation" is used according to Chowning's basic formula (equation 2.1) - this saves a few cycles. 

The usual exponential envelope can feel a bit too distinct and sharp on a cymbal - an open hi hat, for example, can be quite soft. Therefore it is possible to leave the amplitude constant for a while in the beginning. This is done in practice by multiplying the envelope value by a factor and letting the result be limited (can be done automatically in the 56303 processor). 

The usual high pass filter is used, but here twice in series - it simply sounded better (more about the filter in chapter 5.6). The user has the following control possibilities: 

- Frequency (𝑓𝑏 𝐴, 𝑓𝑏 𝐵 = 𝑓𝑏 𝐴 * 1. 411, 𝑓𝑏 𝐶 = 𝑓𝑏 𝐴 * 1. 8, 𝑓𝑏 𝐷 = 𝑓𝑏 𝐴 * 2. 7 ) 

- Decay (decay time of the volume envelope) (𝑑𝑏) 

- Modulator frequency (𝑓𝑚 𝐴, 𝑓𝑚 𝐵 = 𝑓𝑚 𝐴 * 1. 411, 𝑓𝑚 𝐶 = 𝑓𝑚 𝐴 * 1. 8, 𝑓𝑚 𝐷 = 𝑓𝑚 𝐴 * 2. 7 ) 

- Modulation depth (𝐼) 

- Modulator decay (𝑑𝑚) 

- Carrier coupling (𝑏𝑏) 

- Saturation of the volume envelope (𝑆) 

- Cut-off frequency for the high-pass filter (𝑓ℎ𝑝) 

The model described above (see Fig. 4.7) is used for all cymbals (crash, ride, hihat). 

28 



<!-- Start of picture text -->
————/<br>OK}4<br>Sus Inc Ino<br>© & © &<br>is W/ W/ \/ W/<br>Sia a<br>Wike: MosAl os WW] o><br>4 s<br>bt-«<br><]<br>utsignal<br><!-- End of picture text -->

Figure 4.7: Cymbal 

29 

### 5 Implementation 

#### 5.1 Hardware 

Sound generation in Machinedrum is done by a Motorola 56303 DSP, clocked at 100 MHz. This processor should be able to compute 16 different drum sounds simultaneously, with a sampling rate of 44.1 kHz (meaning a maximum of about 130 instructions/sample). Another 56303 handles the mixing of the 16 channels and effects (e.g. low pass, high pass and ring modulation for each channel as well as EQ, delay, reverb and compressor on the mix). The operating system runs on a 40 MHz Motorola 5206 Coldfire processor that communicates with the DSPs via DMA. Tasks include step sequencer<sup>9</sup> , LFOs<sup>10</sup> , interpreting MIDI<sup>11</sup> messages and managing the graphical interface. For the latter purpose there is a 128 x 64 pixel graphic LCD. 16 pushbuttons constitute a step sequencer and for adjusting the character of the drum sounds there are eight " infinite" encoder knobs. 

The connection path includes MIDI in, out and thru, stereo mix out L/R, 4 configurable audio outputs and two audio in. 

#### 5.2 DSP programming 

All drum models described in chapter 4 have been implemented in DSP assembly. As mentioned earlier, the demands on the efficiency of the code were high and therefore it was necessary to exploit the special features of the 56303 as much as possible. These include the ability to move items in the two internal memories in parallel with other instructions. 

#### 5.3 Coldfire programming 

As mentioned earlier, the user of the Machinedrum has the ability to control the sound of each drum using eight knobs. The values of the knobs must be converted in the operating system to appropriate parameter values before being passed on to the sound generating DSP. Each drum model therefore has C code that takes care of things such as the logarithmic response of the frequency knob. 

> 9 An easy way to compose drum rhythms; each beat is divided into 16 steps. Each step has its own button on the drum machine. Pressing a button activates a trigger at the corresponding time. This is done individually for each channel. 

> 10 "Low Frequency Oscillator": slow oscillators that can be linked to different parameters of a synthesizer to create interesting variations in the sound. 

> 11 'Musical Instruments Digital Interface' means a serial interface for communication between electronic musical instruments. 

30 

#### 5.4 The noise generator 

A digital noise generator is really nothing more than a random number generator. There are many ways to generate random numbers, or rather _pseudo_ -random numbers (real ones are much more accurate). The well-known Fibonacci sequence has been used, where the next number in the series is the sum of the previous two: 



a and b are random number seeds. In combination with the modulo count provided in the processor registers, this becomes a well-functioning noise generator. a and b can be chosen arbitrarily as long as both are not 0. However, they should not be too small, as this will result in an oscillation process. 

#### 5.5 Oscillators 

The oscillators were implemented by using a sine table, in this case consisting of 2<sup>15</sup> = 32768 24-bit values. With the 56303 processor, indexing in such a table can be done very efficiently because the special index registers can be set to step forward automatically after each memory access. Each index register also has a modulo register, which can be used for automatic wrapping of the index pointer. 

To achieve the best possible signal-to-noise ratio, the table should actually be much larger - a 24-bit sine table should consist of 2<sup>23</sup> / 2 𝝅 values. Then you are sure that a step forward or backward never means a change of more than 1 LSB. However, such a table takes a lot of space in the memory. An alternative is to reduce the table and interpolate between the values. This can save a lot of space at the cost of small, often almost negligible, degradation of signal quality. The problem is that the interpolation takes up valuable computing power, so I settled for the previously described solution - which worked well enough. 

#### 5.6 The high-pass filter 

The requirements for the filter were not particularly high - the idea was just to be able to slightly attenuate the lower frequencies. A simple first-order high-pass filter proved to work very well (equation 5.1). 





31 

From H(z) we get the following difference equation: 𝑦(𝑛) = 𝑥(𝑛) −𝑥(𝑛−1) + 𝑎𝑦(𝑛−1) (5.2) Equation 5.2 can be easily implemented in DSP assembly. 

32 

### 6 Conclusion and commentary 

In summary, the FM algorithm can be said to be quite usable for the synthesis of drum and percussion sounds. Although realism was not the highest priority in this work, many of the resulting algorithms are capable of producing relatively realistic sounds. 

The tasks included in the thesis are solved, but of course much more work could be done in this area. It would be interesting to try to develop and adapt the existing methods for automatic parameter determination so that they also work for drum sounds. Sophisticated algorithms based on e.g. genetic programming have been developed for FM [8, 9, 10], but they assume that the sound to be imitated is of a more harmonic nature than drum sounds often are. 

33 

### References 

[1] J. M. Chowning, "The Synthesis of Complex Audio Spectra by Means of Frequency Modulation”, _Journal of the Audio Engineering Society_ , vol. 21, nr. 7, pp. 526-534 (1973). 

[2] J. A. Bate, "The Effect of Modulator Phase on Timbres in FM Synthesis”, _Computer Music Journal_ , vol. 14, nr. 3, pp. 38-45 (1990) 

[3] F. Holm, "Understanding FM Implementations: A Call for Common Standards”, _Computer Music Journal_ , vol. 16, nr. 1, pp. 34-42 (1992) 

[4] B. Schottstaedt, “The Simulation of Natural Instrument Tones Using Frequency Modulation with a Complex Modulating Wave”, _Journal of the Audio Engineering Society_ , vol. 1,pp. 46-50 (1977). 

[5] J.-P. Palamin, P. Palamin, "A Method of Generating and Controlling Musical Asymmetrical Spectra”, _Journal of the Audio Engineering Society_ , vol. 36, nr. 9, pp. 671-685 (1988) 

[6] B.T. G. Tan, S. L. Gan, "Real-Time Implementation of Asymmetrical Frequency Modulation Synthesis”, _Journal of the Audio Engineering Society_ , vol. 41, no. 5, pp. 357-363 (1993) 

[7] B.T. G. Tan, S. L. Gan, S. M. Lim, S. H. Tang, "Real-Time Implementation of Double Frequency Modulation (DFM) Synthesis”, _Journal of the Audio Engineering Society_ , vol. 42, nr. 11, pp. 918-926 (1994) 

[8] J. W. Beauchamp, “Synthesis by Spectral Amplitude and Brightness Matching of Analyzed Musical Instrument Tones”, _Journal of the Audio Engineering Society_ , vol. 30, nr. 6, pp. 396-406 (1982) 

[9] A. Horner, J. Beauchamp, L. Haken, "Machine Tongues XVI: Genetic Algorithms and Their Application to FM Matching Synthesis”, _Computer Music Journal_ , vol. 17, nr. 4, pp. 17-29 (1993) 

[10] A. Horner, "Nested Modulator and Feedback FM Matching of Instrument Tones”, _IEEE Transactions on Speech and Audio Processing_ , vol. 6, nr. 4, pp. 398- 409 (1998) [11] Yamaha DX7-II manual 

34 

