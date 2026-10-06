## **CHALMERS** 

Digital drum synthesis TRX for the Elektron Machinedrum SPS-1 DA YID MOLLERSTEDT 

_Signal Processing Group Department of Signals and Systems_ Chalmers University of Technology Goteborg, Sweden, 2004 

EX064/2004 

**lnnehallsforteckning** 

**2.0** --------- **lnnehallsforteckning** 1.0 Abstract 1 2.0 Innehallsforteckning 3.0 Introduktion 2 3.1 Bakgrund och inledning 2 3 .2 Arbetsuppgiften 2 3.3 Problem och syfte 3 4.0 Plattform 4 4.1 Machinedrum SPS-1 4 5.0 Utvecklingsprocess 6 5.1 Analys av TR-808 6 5.2 Oversikt grupper 7 5.2.1 En oscillator 7 5.2.2 Tvii ocsillatorer 7 5.2.3 Brus gruppen 8 5.3 Modell 8 5 .3 .1 En ocsillator 8 5.3.2 Tvii ocsillatorer 9 5.3.3 Brns grnppen 10 6.0 Implementation 12 6.0.1 Sinus 12 6.0.2 Brus 12 6.0.3 Brus genererat friin sex fyrkants viigor. 13 6.1 Anpassning av syntesmodeller 14 6.2 Svarigheter vid implementering 14 7. 0 Resultat och slutsats 16 17 8.0 Litteraturforteckning 9.0 Appendix A 18 9.1 Gamforande studie TR-808 och SPS-1 18 10.0 Appendix B 20 10.1 DSP kod for TRX-CH 20 



**1** 

**lntroduktion** 

### **3.0 lntroduktion** 

##### **3.1 Bakgrund och inledning** 

Under nittonhundratalet introducerades elektroniska musikinstmment. Pa 60-talet tog utvecklingen fart i samband med f<sup>r</sup> amsteg inom elektroniken som mojliggjorde mindre och billigare konstmktioner. Det som vi i dag refererar till som synthar bor­ jade serietillverkas. 

De forsta synthama var helt inriktade mot att li\ta si\ realistiskt som mojligt for att kunna ersiitta de instrument som de hiinnade. Pa 70-talet skapades de forsta syn­ tarna som var renodlade mot att gora tmmljud, de kallades tmmmaskiner. De inne­ holl som regel ocksi\ en sequenser for att kunna i\terspela en takt. Under 70-talet och 80-talet fortsatte utvecklingen av trummaskiner mot allt mer avancerade apparater. 

Under mitten pa 80 tale! slog samplingstekniken igenom, den innebiir att forinspe­ lade ljud spelas upp med valfri hastighet. Samplingstekniken passar sig mycket bra i de fall di\ men vill hiirma trumljud, di\ de iir relativt korta och diirfor inte kriiver orimligt mycket lagringsminne. I och med detta stannade utvecklingen av trum­ maskiner baserade pa elektronisk syntes upp. 

Trots de mi\nga fordelar som finns med den samplingsbaserade tekniken finns det ni\gonting musikaliskt tilltalande i de syntetiskt genererade trumljuden. Detta blev tydligt under 90-talet di\ efterfri\gan okade pa de klassiska trummaskinema som gjorts innan samplingstekniken slog igenom. De smi\ skillnader som finns mellan varje genererat ljud jiimfort med exakta repeterade uppspelningar av en sampling later i ett musikaliskt sammanhang levande. En onskan art skapa trnmljud som inte i forsta hand iir realistiska blev ocksi\ mer framtriidande i samband med de nya musikstilar som viixte fram. Di\ var de syntetiserade trumljuden som iir baserade pa elektriska laetsar mer liimpade iin en sampling av en riktig trumma. 

##### **3.2 Arbetsuppgiften** 

Elektron ESI AB designade 200 l Machinedrum SPS-1 med avsikt att vara den mest avancerade trummaskinen som skapats. En av funktionerna var di\ att digitalt efter­ likna de ljud som analoga syntetiska trummaskiner skapar. Som forebild valdes Roland TR-808. Roland skapade under 80-talet en handful] trummaskiner som ansi\gs mycket avancerade. Det var ocksi\ trummaskiner fri\n denna serie som forst borjade i\terupptiickas niir viljan att anviinda elektroniskt genererade tmmljud som alternativ till samplingar tog fart. TR-808 iir den sista belt analoga trummaskin som skapades i Rolands TR-serie. Totalt best,'\r TR-808 av 11 ljudgenererande kanaler, var och en skapad for att li\ta som en viss typ av trumma. For ni\gra kanaler iir trum­ karaktiiren valbar. 

Avsikten med detta examensarbete iir att skapa en digital syntes som skall kallas TRX och skall inga som en av de fyra syntestyper som ingar i Machinedrum SPS-1. TRX skall i vara baserad pa den ljudgenerering som finns i Roland TR-808. TRX besitta samma ljudkaraktiir som TR-808 och skall vidare erbjuda utokade kon­ rollmojligheter jamfort med den anaolga forebilden. Implementeringen skall ske pa Elektrons ESP plattfrom. 


**2** 

**lntroduktion** 

##### **3.3 Problem och syfte** 

Syftet med TRX ar att fanga de aspekter av den analoga syntetiska forlagan som uppfattas som musikaliskt tilltalande. I vissa fall aterskapas det som en naturlig foljd av att en model! av den analoga kretsen byggs. I andra fall maste modellen paverkas for att uppfora sig pa onskat satt. Uppgiften innehiiller alltsa en subjektiv bedonming av forlagan och den digitala modellen dar i forsta hand de musikaliska aspekterna skall overforas. 

Da en tillfredstallande model! har skapats skall mojligheterna att utvidga den digitala modellen forbi de granser som de analoga konstmktorerna hade utforskas. Det kan gal la utokade omrang for frekvens och tidskontroll over de genererade tmmljuden. 

I TR-808 bidrar vad som skulle kunna anses vara missljud till karaktaren. Yid starka ljudvolymer uppkommer distorsion som far trumljudet att li\ta mer kraftfullt. I den digitala model Jen ar det onskvart att frikoppla denna effekt fran volym installnin­ gen. Det skall ga att ha distorsion pa ett svagt ljud och det skall ga att generera ett kraftigt ljud utan distorsion. 

En de! i uppgiften ar att skapa modeller som utgi\r ifran de som uppskattas i TR-808 men i TRX addera egenskaper som ger okade mojligheter att skapa tmmljud som beh **i** ller samma tmmkaraktar som forlagan. De ljud som skapas av TR-808 har pa gnmd av sin historia pa ett beprovat satt funnit uppskattning. Yid utokning av de ljud som kan skapas kravs att den haller samma kvalitet. 

Syntesmodellen skall implementeras i realtidsmiljo, vilket innebar att signalen inte kan utraknas i forvag utan maste leverera sina utdata inom forbestamd tid. Det finns alltsa en absolut ovre griins for den tid och darn1ed ocksa komplexitet som modellen kan ha. Uppgiften innebar att maximalt utnyttja den tillgangliga ti den for att skapa ctt sa liknande och musikaliskt anvandbart ljud som mojligt. 



**3** 



###### **Plattform** 

### **4.0 Plattform** 

##### **4.1 Machinedrum SPS-1** 

Malplattformen for den syntesmodell som skall utvecklas ar Elektrons Machine­ drum SPS-1. 

###### **FIGUR 1.** 

Elektron Machinedrum SPS-1 



<!-- Start of picture text -->
Peees .'  •'•- E lila  l ' l!!:D:',i;lli�  - =;,w  9 IZIEa :.�: Se 1;,  � � .. \ {:�./ ' t :1 .. •  ; '1 � oe) ••.  •.• :- '� aar;a �  285 � "= • ·�- .•.<br>• �;,t...,.il I  •"(":·.' t:J?')� ■  •  I•  , .y  i   ii I·=,,;:,9.- .-:•,o__,. :• , ,, 1i 1 : (• ·  >' i  . i;.  = :  ·,;_·►.i ••  =,  •·  ..  . · J�__.- - i �c:rc::al?=.. . f  :,:.·',  ■  Iii I.. --?".  •  i   Iii Ii -===-c:i •  • ·  • .' 1  ..  mm, • • .:•  ... 0_ •• · •  ._·,_,  .:..:�•<br>,  :r�..  .• ,. ,  .  ·• ,  ·r,J.,l�. _. ·:'t'\�  ,t  ..  ' \ r •� . ''  - .. , ., :  1  _• ,i  ! - lr '  • •.  •.:? :t�<br>•  ',Y,.·  :·•  •  ·  •  'It  .  ··-�<br><!-- End of picture text -->

Machinedrum ar baserad pa Elektrons ESP plattform. Den bestar av ett proces­ sorkort med en Motorola Coldfire 56206e 40MHz for MIDI, grafik for LCD skarm samt sequenser baserade uppgifter. Vidare finns tva Motorola DSP 56303 samt till­ horande minnessystem. Ett kretskort hanterar audio, dar aterfinns Crystal CS 4334 DA-omvandlare och C1ystal CS 5331 AD-omvandlare kopplade till DSP-systemet via ett I2S granssnitt. Detta kretskort hanterar de analoga in och utgangama pa Machinedrum SPS-1. Ett kretskort for att hantera anvandargranssnittet ar kopplat via ett seriellt interface till Coldfireprocessorn. Dar aterfinns LCD och samtliga ele­ ktromekaniska komponenter. 

TRX programmeras till storsta de! i assembler for DSP-systemet, dock aterfinns vissa delar som ar nara knutna till sequensem i C-kod pa Coldfireprocessom. 

Ljudgenereringen for Machinedrums samtliga 16 parallella trumljud sker i en DSP varefter ljudet fors over digitalt till DSP nummer tva dar effekter laggs pa samt mixning av ljuden sker. 

Varje ljudgenererande enhet har en begransad given tid for att leverera ett resultat. Yid varje anrop skall 32 sampels av audio data levereras, till detta finns maximalt 3200 klockcykler tillgangliga. Machinedrum SPS-1 ar designad som ett hart realtidssystem. Om exekveringstiden vid ett anrop overstiger 3200 klockscykler finns det risk att hela systemet hanger. 



<!-- Start of picture text -->
|<br><!-- End of picture text -->



**4** 

**Plattform** 

Till forfogande for de ljudgenererande enhetema finns minne i fonn av 128 bytes minne for parametrar som behaver sparas mellan anrop, samt 512 bytes minne som delas med de andra syntes modellema dar innehiillet inte sparas till nasta anrop. 





**5** 

**Utvecklingsprocess** 

### **5.0 Utvecklingsprocess** 

##### **5.1 Analys av TR-808** 

Till hjiilp for skapandet at TRX tillhandahiill Elektron en styck TR-808 som under arbetet grundligt utforskades. Till en biirjan genom att spela in, analysera och behandla de ljud som genererades. Denna de! av analysen gick i forsta hand ut pa att fii en grundliiggande fiirstaelse for de ljud som genererades av TR-808. Liingd, frekvensinnehall och dynamisk foriindring analyserades. Det framgick ganska snart att de 11 ljud som TR-808 genererade kunde delas upp i ett antal grupper. Ljud baserade pa en gmndton, ljud baserade pa tva gnmdtoner samt ljud baserade pa fil­ trerat brus. Denna rapport kommer att behandla en specifik model! ur varje grupp. Det praktiska arbetet resulterade i 12 modeller, dock iir manga av modellerna sa snarlika att det inte ar intressant att separat redogiira for samtliga implementeringar. 



#### Roland TR-808 

###### **FIGUR 2.** 



<!-- Start of picture text -->
a Bee tiie Ni Wire iN TS feRoland Nas) |<br>Saracen adSee ahd core hth acon de 2<br>a es | yiad<br>ea PUBS<br><!-- End of picture text -->

Som bas fanns aven logiska kretsschema pa TR-808. Genom analys av dessa kunde Iampliga matpunkter valjas ut. _I_ ett niista steg anvandes dessa miitpunkter for att vidare analysera hur ljudsyntesen i TR-808 sker. Denna analys gav miijlighet att miita pa enskilda delar i de ljudgenererande kretsama. Det gick att analysera ingaende ljud komponenter innan amplitud enveloppen palades. Detta miijliggorde att frekvensbestamma delkomponenter i signaler. Samt att i de fall da det result­ erande ljudet var for kort for en frekvensmatning genomfora matningen pa en altcr­ nativ punkt. 

**TABLE 1.** Ljud som kan genereras av TR-808 per spar 

nr typ engelskt namn svenskt namn 1 BD bassdrum bastrumma 2 SD snaredmm virveltnnnma 

Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 6 

Utvecklingsprocess 

###### **TABLE 1.** 

Ljud som kan genereras av TR-808 per spar 

|nr|**typ**|**engelskt namn**|**svenskt namn**|
|---|---|---|---|
|3|LT/LC|low tom/low conga|lagt stamd puka/Iagt stamd conga|
|4|MT/MC|mid tom/mid conga|mellan stamd puka/mellan stamd conga|
|5|HT/HT|high tom/high conga|hogt stamd puka/hogt stamd conga|
|6|RS/CL|rimshot/claves|kantslag/trapinnar|
|7|CP/MA|handclap/maracca|handklapp/maraccas|
|8|CB|cowbell|koklocka|
|9|CY|cymbal|cymbal|
|10|OH|open hihat|oppen hihat|
|11|CH|closed hihat|stangd hihat|



Tre av kretsarna, de for lag, mellan och hog puka lampade sig mycket val for att kombineras till endast en modell. I TR-808 var installningen for vilka frekvenser som kan valjas den skillnad som finns mellan kretsama. I TRX kunde en modell med utokat frekvensomrade skapas som kunde representera samtliga tre modeller. Samma forntsattning fanns for att kombinera de tre conga modellema till en model I. 

##### **5.2 Oversikt grupper** 

###### **5.2.1 En oscillator** 

Typexemplet for denna gmpp blir bastmmman. Det ar ocksa den medlem i gmppen som har flest installningar. 

**TABLE 2.** 

Typ av trumma for TR-808 och motsvarande typ for TRX 

|**TR-808 typ**|**TRX typ**||
|---|---|---|
|BD|BD||
|LT/LC|XT|XC|
|HT/HC|XT|XC|
|HT/HC|XT|XC|



Ljudet borjar med ett karakteristiskt attack fas, speciell for varje rnodell i denna gmpp, foljt av en ton med bestamd tonhojd. Dar frekvensen som regel har en nagot sjunkande frekvens. I TR-808 ar denna fast och inte kontrollerbar, i den digitala modellen styrs den med tva kontroller. En for mangd f<sup>r</sup> ekvensfall och en for tiden for frekvensandring. 

Den ton som genereras ar sinusliknande. I den digitala modellen genereras den med en tabellerad sinus som moduleras till onskat frekvens innehall. 

###### **5.2.2 Tva ocsillatorer** 

Typexempel for denna grupp blir virveltmmma. 



**7** 

Utvecklingsprocess 

TABLE 3. Typ av trumma for TR-808 och motsvarande typ for TRX 

TR-808 **typ TRX typ** SD SD RS RS CB/CL CB CL 

Ljudet byggs upp av tva toner som via det inbordes intervallet skapar karaktaren for trumman. I fallet virveltrumma inkluderas aven en filtrerad brns signal som repre­ senterar sejannattan i en fysisk trumma 

###### 5.2.3 Brus gruppen 

Exemplifieras av stangd hi-hat. Ljudet byggs upp av brus fran en brusgenerator, som ar uppbyggd av sex fyrkants liknande funktions generatorer implementerade som elektriska vippor i TR-808. Brusct fargas sedan via filtrering. 

TABLE 4. 

Typ av trumma for TR-808 och motsvarande typ for TRX 

TR-808 **typ TRXtyp** CH CH OH OH CY CY MA MA 

###### **5.3 Modell** 

**5.3.1** En **ocsillator** 

Yid undersokningama av hur ljudet upplevs framkom att en stor de! av karaktaren pa ljudet lag i bur attacken upplevdes. Valet foll darav pa att sampla attackfasen och blanda in denna korta vagform med den syntetiserade ton som genererades fran en sinus tabell. Vidare gavs mojlighet till att fonna frekvensinnehallet, tonhojd samt utklingning av tonen i trnmman. 

**TABLE** 5. Kontroller for TRX-8O 

- **nr engelskt namn svenskt namn** PTCH pitch frekvens 

- 2 DEC decay utklingningstid 3 RAMP frequency ramp frekvens forandrings miingd 4 RDEC frequency ramp decay frekvens foriindrings utklingningstid 5 STRT start hardhet vid start 

Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 

8 

**Utvecklingsprocess** 

**TABLE 5.** 

#### Kontroller for TRX-BD 

- nr engelskt namn **svenskt namn** 6 NOJS noise brns vid sta1ien pa ljudet 7 HARM ham1onic content harmoniskt innehall i ljudet 8 CLIP clip klippning av signalen 

**FIGUR 3.** 

#### Block diagram TRX-BD 

TRIG **�** Py TRIG f **-** - ----1 ENVELOPE TRIG TRIG **� �** mR ce **[ PITCH** Jr----. **►** =, **_t\_;_** 5 **5.3.2 Tva ocsillatorer** Karaktaren bestiims till mycket star grad av intervallet mellan de tva toner som ingar. Det visade sig att intervallet skulle vara konstant, iiven da en dynamisk frekvens iindring var onskad. Diiremot var frekvens intervallet en mycket intressant parameter att ge anvandaren kontroll over. Kontroller for TRX-SD nr engelskt nar **n** svenskt nar **n** PTCH pitch frekvens 2 DEC decay utklingningstid 3 SNAP snappy special brus 4 NOIS noise brus 5 TONE tone volym balans mellan occilatorema 6 TUNE tuning frekvens intervall mellan occilartorerna 7 BUMP bump frekvens iindring vid start 8 CLIP clip klippning av signalen **TABLE 6.** 

Karaktaren bestiims till mycket star grad av intervallet mellan de tva toner som ingar. Det visade sig att intervallet skulle vara konstant, iiven da en dynamisk frekvens iindring var onskad. Diiremot var frekvens intervallet en mycket intressant parameter att ge anvandaren kontroll over. 

###### **TABLE 6.** 



**9** 

##### **Utvecklingsprocess** 



<!-- Start of picture text -->
Block diagram TRX-SD<br>TRIG<br>I---<br>TRIG<br>�t<br>ENVELOPE  1---<br>TRIG  TRIG<br>I--- D'<br>�  . �  « HES<br>PITCH<br>t\__;  -<br>TRIG<br>�<br>ran .___<br>t\__;<br>5.3.3  Brus gruppen<br>FIGUR 4.<br><!-- End of picture text -->

Filtreringen av bruset bar i denna grupp prioritet. De filter som kravs bar inga krav pa sig att vara faslinjara dii insignalen ar brus. IIR filter valdes da de ar imple­ menterings massigt fordelaktiga. Lagpassfiltereing sker med ett 24 dB/oktav brant 



<!-- Start of picture text -->
TABLE  7.  Kontroller for TRX-CH<br>nr  engelskt namn  svenskt  namn<br>GAP  gap  mellanrum mellan hihat<br>locken<br>2  DEC  decay  utklingningstid<br>3  LFP  lowpass frequency  lagpass frekvens<br>4  HPF  bipass frequency  hogpass frekvens<br>5  PEAK  peak  resonans for hog och lagpass<br>filter<br>6  MTAL  metal  metallisk klang i ljudet<br><!-- End of picture text -->



**1 o** 

###### **Utvecklingsprocess** 

filter dar saval brytfrekvens som resonans ar kontrollerbart av anvandaren. Hog­ passfiltereing sker med tva 12 dB/oktav branta filter, de ena filtet ar fast for att i alla Iagen ta bort en viss mangd av basfrekvenser, de andra filteret ar svepbart. 

**FIGUR 5.** 

Block diagram TRX-CH 



<!-- Start of picture text -->
TRIG<br>m}HAHEH<br>ENVELOPE<br>fe<br><!-- End of picture text -->











**Digital trurnsyntes TRX for Elektron Machinedrurn SPS-1 EX064/2004** 

**11** 

**Implementation** 

### **6.0 Implementation** 

----------- 

**TABLE 8.** ; mask out relevant bits ; do this now to prevent pipeline stall ; calculate new osc I pos nop x:(r2+sintab ),a ; fetch sample #2,a,a nop ; do the actual saving move bl,y:(r6+BDX80 8_OSC1POS) ; save position move b0,y:(r6+BDX808_ OSC I POS+ I); save position #32768-1 ,b punkter. Storleken pa sinustabellen var en given designparameter da den anvands av ett flertal delar av Machinedrum SPS-1 samt Jigger i delat minne som begransar and move add nop move move a,x:(r4)+ **6. 0. 1 Sinus** bl,r2 bdx808_sin_loop Samtliga toner som anvands ar genererade fran en 24-bitars sinustabell pa 4096 move y:(r6+BDX808_OSC1POS),b ; get position move y:(r6+BDX80 8_OSC1POS+J),b0 asr x,b dess storlek. do #32,bdx80 8_sin_loop Kod for sinustabell move al,xl move a0,x0 

**TABLE 9.** Kod for brus ;; calculate 32 samples of noise and place in free_x move #>free_x+CH808_NOISE_BLOCK,r5 move #3 l ,m5 move y:(r6+CH808_NOISE_A),b move y:(r6+CH808_NOISE_B),xl do #3 l ,ch808 _ noise _loop add xl,b ; "Rotate" move xl,x0 ;xi-> temp ymokning. Samma argument galler for de modeller som har flera bruskallor intemt. korrelationen gjort att det inte hade upplevts som flera trummor utan som en vol­ flera parallella instanser av samma model!. Hade bruset genererats centralt hade Brus genereras lokalt via en slumpgenerator. I Machinedrum SPS-1 kan man kora **6.0.2 Brus** 



**Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 12** 

##### **Implementation** 

move bl,x:(r5)+ ; do the actual saving bl,xl ;bl -> xi move ;temp -> bl ch808 _noise _loop add xl,b ; dont forget to do the last add move xl,y:(r6+CH808_NOISE_A) ; save seeds while "rotating" them move b l ,y:(r6+CH808 _NOISE_ B) move b l ,x:(r5)+ ; do the actual saving move x0,bl 

; we could take the noise in reverse order but this f **e** ls 'stable' 

; we're done with the noise gen 

**6.0.3 Brus genererat frirn sex fyrkants vagor.** For att ge bruset ri:itt karakti:ir var det viktigt att skapa det pa samma si:itt som i origi­ nalet. Forst prtivades en modell di:ir sinusvagor klipptes till fyrkantsvagor. Det gav ett resultat som lat mycket likt originalet. Tyvi:irr var det allt for processorintensivt for att kunna anvi:indas i den slutgiltiga implementeringen. Da det inte fanns plats i minnet for en fyrkants tabell av samma storlek som sinustabellen fick en alternativ metod skapas. Lyckligtvis var frekvenserna fasta och en tabell av minimal storlek kunde anvi:indas. 

**TABLE 10.** Kod for fyrkantsvagor repeterad 6 ganger move y:(r6+CH808_ COUNT2),b ; Load current pos move #sqr_table2,y0 ; Load table pos add y0,b ; calc effective adress move y:(r6+CH808_OUT2),x0 ; Load current out-value move b,r3 ; load r3 with adress move #SQR_FREQ2-l ,m3 do #l 6,ch808_1oop2 move x:(r3)+,xl ; load from table move x:(r3)+,yl ; load from table x0,xl,a x:(r4)+,xl ; calc out value & load old sum x0,yl,b x:(r4)+,y1 ; calc out value & load old sum add xl,a add yl,b move a,x:(r5)+ ; Save out move b,x:(r5)+ ; Save out ch808_loop2 move r3,b ; load new adress sub y0,b ; calc pos, table pos already in y I nop move b,y:(r6+CH808_ COUNT2) ; Save current pos **TABLE 11.** Tabell frekvenser fyrkantsvager ; sqr freqs SQR_FREQJ equ 114 ; this is for approx. freq 306 @ 44100 SQR_FREQ2 equ 102 ; values are for a whole period432 mpy mpy 



**Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 13** 

# **Implementation** — 

SQR_FREQ3 equ 86 512 SQR_FREQ4 SQR_FREQ5 826 SQR_FREQ6 551 equ 80 equ 58 equ 52 

##### **6.1 Anpassning av syntesmodeller** 

Ur de tre huvudgruppema skapades totalt 12 syntes modeller. 

|**TABLE 12.**|TRX||||
|---|---|---|---|---|
||**nr**|**typ**<br>BD|**engelskt namn**<br>bassdrum|**svenskt namn**<br>bastrumma|
||2|SD|snaredrum|virveltrumma|
||3|XT|toms|pukor|
||4|XC|congas|kongas|
||5|RS|rimshot|kantslag|
||6|CP|handclap|handklapp|
||7|CL|maracca|tdipinnar|
||8|CB|cowbell|koklocka|
||9|CH|closed hihat|stlingd hihat|
||JO|OH|open hihat|oppen hihat|
||**]]**|CY|cymbal|cymbal|
||12|MA|maracca|maraccas|



Detta genom att eventuell volym som ej avklingat fran tidigare tillslag adderades till Samtliga modeller gavs sedan ett volymtillskott vid tidsmassigt narliggande triggar. en grundvolym som ligger pa 85% av maximal volym. 

trar som gar att kontrollera via sequensem maste beaktas sa att de far onskad effekt i varje de) i trigfaserna. Det innebar att samtliga anvandarstyrda parametrar mi\ste Yid integrationen av syntesmodellerna till den miljo dar de skall exekveras styrda kontinuerligt uppdateras, det ar inte tillrackligt att liisa in parametrar vid av sequensem i Machinedrum SPS-1 kravdes ytterliggare anpassning. De parame­ trigogonblicket. 

##### **6.2 Svarigheter vid implementering** 

anvandas da frekvenserna lag fasta. Hade rorliga frekvcnser varit ett krav hade en den ursprungliga tekniken med hardklippta sinus vi\gor. Lyckligtvis gav metoden med tabellerade fyrkantsvilgor ett fullgott resultat. Denna metod kunde med fordel Generering av de sex fyrkantsvagoma visadc sig vara for processorintensivt enligt annan metod behovt anvandas. 

Yid aterstart av en trumma som ar baserad pa en sinusvag och inte har klingat ut kan ett knapp uppsta om vagformen startas om. Det ar onskvart i de fiesta fall att ater­ starta vagformen, annars upplevs som att slaget inte riktigt traffar. Speciellt galler 



**Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 14** 

###### **Implementation** 

det vid korta slag vid lag frekvens da bara en kort de! av vagfonnen spelas upp innan Jjudet har klingat ut. For att undvika missljud vid atertrig och kunna aterstarta vagfom1en infordes en utklingningsfas innan slaget aterstartades. En mycket snabb volym sankning genomfordes, dock med <let urspr **n** gliga volym vardet sparat for att kunna bidra till volymen pa nasta slag enligt ovan. 



**15** 

**Resultat och slutsats** 

### **7.0 Resultat och slutsats** 

TRX ar en digital modell baserad pa den analoga elektriska forebilden TR-808. TRX ar utokad i hanseende pa kontrollmojligheter. Ljudkaraktaren fran den klas­ siska forebilden i kombination med de nya mojligheterna till ljudskulptering ar den aspekt av TRX som har fatt mest uppskattning. 

Trots de olika tekniker som har anvants intcmt har TRX for anvandaren ett homo­ gent framtradande. 

For en jamforande studie se Appendix A. TR-808 har vid denna jamforelse stall ts med de kontroller som finns i mittlage, SPS-I har stall ts in for att ha ett liknande Ijud. 

I kombination med de andra syntestypema har Machinedrum SPS-1 blivit en mycket komplett och uppskattad produkt. Den har sedan forsaljningsstarten 2001 fram till dagens datum salt 1481 enheter. 





**16** 

### **8.0 Littera**<sup>**turforteckning**</sup> 

Motorola, DSP56300 Family Manual, Order number DSP56300FAM/AD. 

Watkinson John, The Art of Digital Audio, Focal Press, ISBN: 02405187. 

l 3-75<sup>4920-2</sup> Oppenheim A. V. & Schafer R. W. 1998. Discrete-time Signal Processing. ISBN O­ 

Manufac<sup>tureres Association</sup> The Complete MIDI 1.0 Detailed Specification, Document version 96.1, The MIDI 



**Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 17** 



<!-- Start of picture text -->
—————<br>Appendix A<br>---------------- 9.0  Appendix A<br>9.1  Jamforande  studie  TR-808  och SPS-1<br>FIGUR 6.  BD TR-808 - SPS1 jamforelse<br>Stanmore mo OOD nes ETa0 STE AEESSPSS Reed<br>(Samples Pgcae Age oon SR ONO es EP ooo eames a<br>[tr 000 [e)M)> [eros va-01 7<br>eyes ih |<br>ele} || |<br>oeTIS] IE Fy| ee<br>Naretern Dy |<br>raat reed |<br>canes |) |<br>FIGUR 7, SD TR-808 - SPS-1 jamfbrelse<br>‘200 11808_sps1<br>faeskate omef° "Spor S18) ere)2h  SEMFee Pee ewe wT ap 107|| i39808| stat 20; PRR SRR RY<br>peremeencermj~ Te | AO~rae| o>O101010)fas [pia 0 SS ARCS ord EM10000 7] NasAT9000 7] Corser ETT46000 pore“8108064 eeeee. as<br>prae] Be [ooo ss wrasse Ys3000Te ene Nat | e2000thes Nae eA 48000. Fanasts De Tf o4000, PPA CTS asoOo Sg Teeaoo ——<br>|<br>evoee<br>ey<br>fot ert ane Whiter |<br>Sat ————<br>eel<br>exon |<br>ciate! [9 |<br>iB tel Hi<br>ie a |i<br>.<br>it<br>TT<br><!-- End of picture text -->



Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 18 



<!-- Start of picture text -->
a<br>Appendix A<br>een<br>ae 70 7<br>aoe Deeae<br>CH TR-808 - SPS-1 jamf6relse<br>pesca8008BE eh Pe ge 2 eT BPELNDP PLS1r808_sps1 CISDat RS EENEE NR<br>Mera MO pss pa ko lps elena | grea aig ass Tepe stein<br>eemeccamnmerreey LDCR Re BU ae Tao Ra Ss EAS ASD<br>EFMinsSecsic: 2] 10:1.00 8 sak sesisisa O}LLOOEIY NE D:POONARRLILATTO:1.00 TSE RRL LUA DA LOAF AIRS OOO NOL.OOBWELSER —_0:1.00<br>eeReG00 [eM | w200 aos ||<br>elerel ace] ' ; |<br>ore) e)M<br>Laverorm >} |<br>{ress |<br>chee) |<br>a! i<br>FIGUR 8.<br><!-- End of picture text -->



Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 

19 

**Appendix B** 

### **10.0 Appendix B** 

##### **10.1 DSP kod for TRX-CH** 

; Calculate a buffer of 32 sound data ; r6 = pointer to data ; r7 = pointer to sound data tr808_ch_plugin ;; calculate 32 samples of noise and place in free_ x move #>free_x+CH808_NOISE_BLOCK,r5 move#31,m5 move y:(r6+CH808_NOISE_A),b move y:(r6+CH808_NO1SE_B),x I do #3 l,ch808_noise_loop add xl,b ; "Rotate" move x l,x0 ; xi -> temp move bl ,x:(r5)+ ; do the actual saving move bl,xl ;bl-> xl move x0,bl ; temp-> bl ch808 _noise _loop add xl,b ; dont forget to do the last add move xl,y:(r6+CH808_NOISE_A) ; save seeds while "rotating" them move bl ,y:(r6+CH808 _NOISE_ B) move bl ,x:(r5)+ ; do the actual saving ; we could take the noise in revers order but this f **e** ls 'stable' 

; we're done with the noise gen 

;;; apply volume to the noise to control the dist in the I<sup>p;;;</sup> 

move y:(r6+CH808_NOISE_ VOL),x0 

do # l 6,ch808 _ vol move x:(r5)+,y0 move x:(r5)-,yl mpy y0,x0,a ; apply vol mpy yl,x0,b ; apply vol move a,x:(r5)+ ; save sample move b,x:(r5)+ ; save sample ch808_vol 

;;; vol done;;; 

S3a3anaananazba9 099009999999 UO SIX SQTS ,,,,,,,,,,,))))))))))))))) ceeersreeeesetetretetteees 

**20** 



**Appendix B** 

move #>free_x+CH808_NOISE_BLOCK,r4 ; both r4 and r5 points to SQR_BLOCK move #3 1,m4 



<!-- Start of picture text -->
3553553535809 MO] 535555ss5a53535005005<br><!-- End of picture text -->

move y:(r6+CH808_COUNT1),b move #sqr _ table l ,y0 add y0,b move y:(r6+CI-I808 _ OUT 1 ),x0 move b,r3 move #SQR_FREQl-l,m3 

; Load current pos 

; Load table pos 

; calc effective adress 

; Load current out-value 

- ; load r3 with adress 

do # l 6,ch808 _loop 1 move x:(r3)+,x 1 move x:(r3)+,yl mpy x0,xl,ax:(r4)+,xl mpy x0,yl,bx:(r4)+,yl add xi ,a add yl,b move a,x:(rS)+ move b,x:(r5)+ ch808_1oopl move r3,b sub y0,b nop move b,y:(r6+CH808_COUNT!) 

; load from table 

; load from table 

; calc out value & load old sum 

; calc out value & load old sum 

; Save out 

; Save out 

; load new adress 

; calc pos, table pos already in yl 

; Save crnTent pos 



move y:(r6+CH808 _ COUNT2),b move #sqr_table2,y0 add y0,b move y:(r6+CH808 _ OUT2),x0 move b,r3 move #SQR _FREQ2-I ,m3 do # l 6,ch808 _loop2 move x:(r3)+,xl move x:(r3)+,yl mpy x0,xl,ax:(r4)+,xl mpy x0,yl,bx:(r4)+,yl add xl,a add yl,b move a,x:(r5)+ move b,x:(rS)+ ch808 _loop2 move r3,b sub y0,b nop move b,y:(r6+CH808_COUNT2) 

; Load current pos 

- ; Load table pos 

; calc effective adress 

- ; Load ctment out-value 

- ; load r3 with adress 

; load from table 

; load from table 

; calc out value & load old sum 

; calc out value & load old sum 

; Save out 

; Save out 

; load new adress 

; calc pos, table pos already in yl 

; Save cunent pos 

,,,,,,,,,,,,, ............ no 3 ..................... . **,,,,,,,,,,,,))))))))))** 



**21** 

**Appendix B** 

move y:(r6+CH808_ COUNT3),b move #sqr_table3,y0 add y0,b move y:(r6+CH808_OUT3),x0 move b,r3 move #SQR_FREQ3-l,m3 do # J 6,ch808 _loop3 move x:(r3)+,x 1 move x:(r3)+,yl mpy x0,xl,ax:(r4)+,xl mpy x0,yl,bx:(r4)+,yl add xl,a add yl,b move a,x:(rS)+ move b,x:(rS)+ ch808_loop3 move r3,b sub y0,b nop move b,y:(r6+CH808_COUNT3) ;;;;;;;;;;;;,<sup>no 4</sup> ;;;;;;;;;;;;;;;;;;;;;; move y:(r6+CH808_COUNT4),b move #sqr_table4,y0 add y0,b move y:(r6+cH808 _ OUT4),x0 move b,r3 move #SQR _FREQ4-l ,m3 do # J 6,ch808 _loop4 move x:(r3)+,x 1 move x:(r3)+,yl mpy x0,xl,ax:(r4)+,xl mpy x0,yl,bx:(r4)+,yl add xl,a add yl,b move a,x:(r5)+ move b,x:(rS)+ ch808_1oop4 move r3,b sub y0,b nop move b,y:(r6+CF-I808_COUNT4) 

- ;<sup>Load current pos</sup> 

- ; Load table pos 

- ;<sup>calc effective actress</sup> 

- ;<sup>Load current out-value</sup> 

- ;<sup>load r3 with actress</sup> 

- ;<sup>load from table</sup> 

;<sup>load from table</sup> 

- ;<sup>calc out value & load old sum</sup> 

- ;<sup>calc out value & load old sum</sup> 

- ;<sup>Save out</sup> 

;<sup>Save out</sup> 

;<sup>load new actress</sup> 

;<sup>calc pos, table pos already in y 1</sup> 

;<sup>Save current pos</sup> 

- ;<sup>Load current pos</sup> 

- ;<sup>Load table pos</sup> 

;<sup>calc effective actress</sup> 

- ;<sup>Load current out-value</sup> 

- ;<sup>load r3 with actress</sup> 

;<sup>load from table</sup> 

;<sup>load from table</sup> 

;<sup>calc out value & load old sum</sup> 

;<sup>calc out value & load old sum</sup> 

;<sup>Save out</sup> 

;<sup>Save out</sup> 

;<sup>load new actress</sup> 

;<sup>calc pos, table pos already in yl</sup> 

;<sup>Save current pos</sup> 

;;;;;;;;;;;;<sup>, no 5</sup> ;;;;;;;;;;;;;;;;;;;;;; 

move y:(r6+cH808_COUNT5),b ;<sup>Load cunent pos</sup> move #sqr_table5,y0 ;<sup>Load table pos</sup> add y0,b ;<sup>calc effective actresss</sup> move y:(r6+CH808_OUT5),x0 ;<sup>Load ctment out-value</sup> move b,r3 ;<sup>load r3 with actress</sup> 

;<sup>calc effective actresss</sup> 

;<sup>Load ctment out-value</sup> 





<!-- Start of picture text -->
~~<br><!-- End of picture text -->

**22** 

**Appendix B** 

move #SQR_FREQ 5-1,m3 

do # 1 6,ch808 _loop5 move x:(r3)+,xl move x:(r3)+,yl mpy x0,xl,ax:(r4)+,xl mpy x0,yl,bx:(r4)+,yl add xl,a add yl,b move a,x:(r5)+ move b,x:(r5)+ ch808_1oop5 move r3,b sub y0,b nop move b,y:(r6+CH808  COUNTS)_ 

- ; load from table 

; load from table 

; calc out value & load old sum 

- ; calc out value & load old sum 

; Save out 

   - ; Save out 

- ; load new adress 

- ; calc pos, table pos already in y 1 

   - ; Save current pos 



<!-- Start of picture text -->
Spansannsaase MO 6 j55553;53sanNHNINIITT<br><!-- End of picture text -->

move y:(r6+CH808_ COUNT6),b move #sqr_table6,y0 add y0,b move y:(r6+CH808_OUT6),x0 move b,r3 move #SQR_FREQ 6- l ,m3 

- ; Load current pos 

- ; Load table pos 

- ; calc effective adress 

- ; Load current out- value 

- ; load r3 with adress 

do # I 6,ch808 _loop6 move x:(r3)+,xl move x:(r3)+,yl mpy x0,xl,ax:(r4)+,xl mpy x0,yl,bx:(r4)+,yl add xi ,a add yl,b move a,x:(r5)+ move b,x:(r5)+ ch808_1oop6 move r3,b sub y0,b nop move b,y:(r6+CH808_COUNT6) 

- ; load from table 

- ; load from table 

- ; calc out value & load old sum 

- ; calc out value & load old sum 

; Save out 

- ; Save out 

; load new adress 

- ; calc pos, table pos already in yl 

; Save current pos 

;;;;;;;;;;;;;;;;; **done** Vlith **the sqrs** ;;;;;;;;;;;;;;;;;;;;; 

;;;;;;;;;;;;;; **filter away** ;;;;;;;;;;;;;;;;;;;;;;;;;;;; ;---Lowpass fi 1 ter -------------------------------------------- 

; design the lowpass filter move y:(r6+CH808_LPFREQ),a move y:(r6+CH808_LPQ),r2 

move #free _y+CH808 _ VOLi ,r4 move#l,m4 

; pointer to volume coefficient (a*x) and b2 

**Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 23** 

**Appen**<sup>**dix B**</sup> 

mo<sup>ve x:(r2+csqtab ),xO</sup> m<sup>ove x:(r2+eqtab),xl</sup> add<sup>x0,a al,b</sup> m<sup>o</sup> v<sup>e #free_x+CH808_NOISE_BLOCK,r0; from buffer</sup> m<sup>ove#-J,m0</sup> add<sup>xJ,b al,rl</sup> mov<sup>e #free_x+CH808_NOISE_LPQ 1,r3</sup> ; sla<sup>pande ifrombuffer, NOISE_BLOCK-2</sup> m<sup>ove #-J,m3</sup> mov<sup>e b1,r2</sup> m<sup>ove #2,n0</sup> ; g<sup>e</sup> t<sup>values according to freq</sup> m<sup>ove x:(rJ+cstab+2048),xl</sup> ; ge<sup>tvalues according to freq</sup> mo<sup>ve x:(r2+etab+2048),yl</sup> mo<sup>ve #free _x+CH808 _HISTORY J ,r I; pointerto history</sup> mov<sup>e#J,ml</sup> mpy<sup>xl ,yl,a yJ,x0; e*cs</sup> mmpy -<sup>ove #freex0,yl ,b _x+CH808</sup> ; -e*<sup>e _NOISE_LPQ2,r2</sup> ;<sup>slapande i frombuffer, NOISE_BLOCK-1</sup> mov<sup>e #-1,m2</sup> ; 2<sup>*e*cs</sup> as!<sup>ab,xl</sup> m<sup>o</sup> v<sup>e y:(r6+CH808_LPOLDY2),y1</sup> mo<sup>ve yJ,x:(rJ)+</sup> m<sup>ove y:(r6+CH808_LPOLDY1),yl</sup> m<sup>ove yJ,x:(rJ)+</sup> a,x<sup>0</sup> ad<sup>d xJ,a</sup> ; J<sup>-2 *e*cs+e*e = norm</sup> sub<sup>#0.50,a</sup> n<sup>e</sup> g<sup>a</sup> mo<sup>ve b,y:(r4)+</sup> ; i<sup>nit b2</sup> m<sup>ove b,yl</sup> m<sup>ove a,y:(r4)</sup> ; a<sup>O</sup> ; r<sup>0: input pointer</sup> ; r<sup>1: history buffer for y-J ,y-2</sup> ; r<sup>2: pointer to z-1, (r0-1)</sup> ; r<sup>3: pointer to : pointer to inter to ter toz-2, (r0-2)</sup> ; r<sup>4: pointer to a,b2 ointer to a,b2</sup> eee **Dig**<sup>**ital trumsyntes TRX tor Elektron Machinedrum SPS-1 EX064/2004**</sup> 

; r<sup>3: pointer to : pointer to inter to ter to</sup> ; r<sup>4: pointer to a,b2 ointer to a,b2</sup> ee **Dig** 



**24** 

hao **App**<sup>**endix B**</sup> 

move y<sup>0,x:(r3)</sup> move y<sup>:(r6+CH808_LPOLDZ1),y0</sup> move<sup>y0,x:(r2)</sup> move y:(r6+CH808_LPOLDZ2),y0; load old val ues 

, • filter th e e sig<sup>nal (24 bi t, 24 dB/oct ver sion)</sup> 

; get y<sup>-2</sup> mov<sup>ex:(rJ)+,xl</sup> xJ,yl,<sup>a</sup> ; get y<sup>-l</sup> x:(r0)+,xl y:(r4)+,y0 ; y<sup>-</sup> lgetx geta0 x0,yl,<sup>a</sup> mac ;<sup>a0*x</sup> get z<sup>-</sup> 1 mac xJ,y0,<sup>a</sup> mpy x:(rl)+,yl 

do #32,<sup>ch808_lpFil terLoop</sup> 

> a s! xJ,ya<sup>l,b</sup> x:(r3)+,x I x:(r2)+,yl y:(r4)+,yl ; get z-1 ; shift getb2<sup>a</sup> nd z2 a,x l y:(r4)+,y0 ; z<sup>-</sup> 1 get y x0,yl,b ma c a, x:(rl)+ get y-1 b x:(rl)+,xl y:(r4)+,yl ; s<sup>shift get b2</sup> 

> a s! x:(rl)+,yl ; get y<sup>-1</sup> xJ,yl,<sup>a</sup> mpy x:(r0)-,xl y:(r4)+,y0 ;y<sup>-</sup> lgetx<sup>get a0</sup> x0,yl,<sup>a</sup> mac b,x:(r0)+n0 ;<sup>a0*x</sup> get z<sup>-</sup> 1 mac xJ,y0,<sup>a</sup> mpy ; a0*y 

ch808<sup>_IpFil terLoop</sup> 

; r<sup>J points a t y-2</sup> 

move<sup>x:(rJ)+,yl</sup> move y<sup>J,y:(r6+CH808_LPOLDY2)</sup> move x<sup>:(rl)+,yl</sup> move y<sup>l ,y:(r6+CH808_LPOLDYJ)</sup> 

move<sup>x:(r2)-,yl</sup> move y J<sup>,y:(r6+CH808_LPOLDZ 1)</sup> move<sup>x:(r2),yl</sup> move yl<sup>,y:(r6+CH808_LPOLDZ2)</sup> 

; re<sup>return to o riginal sta te</sup> 

···········••••••• LP don e e ,,,,,,,,,,,,<sup>,,,,,,</sup> ;;;;;;;;;;;;<sup>;;;;;; now HP</sup> ;;; hp<sup>no 1</sup> 

;---H ipa s ss fi Jter --------------------------------------------- 

———— a **Digital trum**<sup>**syntes TRX for Elektron Machinedrum SPS-1 EX064/2004**</sup> **25** 

as **Appendix B** 

; design the hipass filter 

move y:(r6+CH808 _ HPFREQ),a move y:(r6+CH808_HPQ),r2 move #free_y+CH808_ VOLJ,r4 ; pointer to volume coefficient (a*x) and b2, five of them move #4,m4 move x:(r2+csqtab),x0 move x:(r2+eqtab ),x I add x0,a al,b move #free _x+CH808 _NOISE_ LPQ l ,r0 ;; sliipande i from buffer, NOTSE_BLOCK-2 move #-1,m0 add xl,b al,rl move y:(r6+CH808_HPOLDX2),xl move x I ,x:(r0)+ move bl,r2 move y:(r6+CH808_HPOLDXl),xl move x J ,x:(r0)move x:(rl+cstab+2048),xl move x:(r2+etab+2048),yl move #free_x+CJ-I808_NOISE_HPQ2,rl move #-1,ml mpy xl,yl,a yl,x0; e*cs mpy -x0,y l ,b ; -e*e move #free _x+CH808 _NOISE_ HPQ I ,r2 move #-l,m2 as! a ; 2*e*cs move y:(r6+CH808_HPOLDY2),yl move yl,x:(r2)+ move y:(r6+CH808_HPOLDYl),yl move yl,x:(r2) ;add xl,a a,x0 ;sub #0.50,a ; l-2*e*cs+e*e = 11om1 ;neg a move #0.5,x I move xl,y:(r4)+ 

**_Digital_ trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/20o4** 

**26** 

##### **Appendix B** 

move _#-1.0,yO_ move y0,y:(r4)+ move xl,y:(r4)+ move a,y:(r4)+ move b,y:(r4)+ 

; filter the signal (24 bit, 12 dB/oct version) movex:(r2)-,a ; get _y-1_ movex:(r0)+,x0 y:(r4)+,y0 do # I 6,ch808_ hpFilterLoop x0,y0,b x:(r0)+,x0 ma c x0,y0,b x:(r0)-,x0 mac x0,y0,b x:(r2)+,x0 ma c x0,y0,b a,x0 ma c x0,y0,b x:(r0)+,x0 as! b a,x:(rl)+ x0,y0,a x:(r0)+,x0 mac x0,y0,a x:(r0)-,x0 y:(r4)+,y0 mac x0,y0,a x:(r2)+,x0 y:(r4)+,y0 mac x0,y0,a b,x0 y:(r4)+,y0 mac x0,y0,a x:(r0)+,x0 as! a b,x:(rl)+ mpy mpy y:(r4)+,y0 y:(r4)+,y0 y:(r4)+,y0 y:(r4)+,y0 y:(r4)+,y0 

; get _y-1_ 

ch808 _ hpFilterLoop 

; rl points at y-2 

move x:(r2),yl move yl ,y:(r6+CH808_HPOLDY 2) move a,y:(r6+CH808_HPOLDYl) move a,x:(rJ)+ 

move x:(r0),yl move yJ,y:(r6+CH808_HPOLDXl) move x:-(r0),y l move _yl_ ,y:(r6+CH808_HPOLDX2) 

;;;hp no 2 

;---H ipass filter --------------------------------------------- 

; design _the_ hipass filter 

move y:(r6+CH808_HPFREQ_ 2),a move y:(r6+CH808_HPQ_2),r2 

**_Digital_ trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004** 

**27** 



<!-- Start of picture text -->
Se<br><!-- End of picture text -->



**_Appendix_ 8** 

move #free_y+CH808_ VOLl,r4 ; pointer to volume coefficient (a*x) and b2, five of them move #4,m4 move x:(r2+csqtab),x0 move x:(r2+eqtab),xl add x0,a al,b move #free_x+CH808_NOISE_HPQ1,r0 ;; slapande i from buffer, NOISE_BLOCK-2 move #-1,m0 add xl,b al,rl move y:(r6+CH808_HPOLDX2_2),xl move _x_ I ,x:(r0)+ move bl,r2 move y:(r6+CH808_HPOLDXI_2),xl move _x_ I ,x:(r0)move x:(rl+cstab+2048),xl move x:(r2+etab+2048),yl ;; this is the out buffer from hp 1 

move #free_x+CH808_NOISE_LPQ2,rl ; we are using the Ip buffer to write _to_ now move #-!,ml mpy xl,yl,a yl,x0; e*cs mpy -x0,yl,b ; -e*e move #free_ x+CH808 _NOISE_ LPQ l ,r2 move #-J,m2 as! a ; 2*e*cs 

move y:(r6+CH808_HPOLDY2_2),yl move yl , x:(r2)+ move y:(r6+CH808_HPOLDYl_ 2),yl move yl,x:(r2) 

;add xl,a a,x0 ;sub #0.50,a ; I-2*e*cs+e*e _=_ norm ;neg a move #0.5,x I move xl,y:(r4)+ move #-1.0,y0 move y0,y:(r4)+ move xl,y:(r4)+ move b,y:(r4<sup>)+</sup> move a,y:(r4)<sup>+</sup> 

---------------- 

- **-** -------- 

**_Digital_ trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004** 

**28** 

**Appendix B** 

'• filter the signal (2 4 bit, _12 dB/act_ version) 

movex:(r2)-,a movex:(r0)+,x0 y:(r4)+,y0 

; get _y-1_ 

; get x and a0 

|do # J 6,c<br>mpy<br>mac<br>mac<br>mac<br>mac<br>asl|h808 _hpFilterLoop_2<br>x0,y0,b<br>x0,y0,b<br>x0,y0,b<br>x0,y0,b<br>x0,y0,b<br>b|x:(r0)+,x0<br>x:(r0)-,x0<br>x:(r2)+,xo<br>a,xo<br>x:(r0)+,x0<br>a,x:(rJ )+|y:(r4)+,y0<br>y:(r4)+,yo<br>y:(r4)+,yo<br>y:(r4)+,yo<br>y:(r4)+,y0|
|---|---|---|---|
|mpy<br>mac|x0,y0,a<br>x0,y0,a|x:(r0)+,x0<br>x:(r0)-,x0|y:(r4)+,y0<br>y:(r4)+0|
|mac<br>mac<br>mac|x0,y0,a<br>x0,y0,a<br>x0,y0,a|x:(r2)+,x0<br>b,x0<br>x:(r0)+,x0|,y<br>y:(r4)+,yo<br>y:(r4)+,y0<br>y:(r4)+o|
|asl|a|b,x:(rl )+|,y|



ch808 _hpFilterLoop _ 2 

; rl points at y-2 

move x:(r2),yJ move yJ ,y:(r6+CH808_HPOLDY2_2) move a,y:(r6+CH808_HPOLDYJ_2) move a,x:(rl)+ 

move x:(r0),yl move yl,y:(r6+CH808_HPOLDXJ_2) move x:-(r0),yl move yl,y:(r6+CH808_HPOLDX2_2) 

**;;;;;;;;;;;;;;done** **_with_ the filter;;;;;;;;;;;;;;;;;;;;;;;;** 

NOISE>BEOSKCH808 NOISE BLOCK, dm;,vefrfi **l** i ,.SJ is in NOT SE _ BLOCK2 ; the N6>J8F1�RM2_<xKCH808_NOISE_BLOCK,r5; the out data from hp 2 is in 

move y:(r6+CH808_AMP _ENV),b 

l.:ftn ; any beq ch808_decay ; no, jump to decay sub #>l,a nop move a,y:(r6+CH808 _HOLD_ TIME) **Digital trumsyntes TRX for Elektron Machinedrum SPS-1 EX064/2004 2g 1111** Be move y:(r6+CH808_HOLD _ TIME),a; nr of frames to leave amplitude at max 



<!-- Start of picture text -->
OT<br><!-- End of picture text -->

**App**<sup>**endix B**</sup> os bra<sup>ch808_holdout</sup> ch808<sup>_ decay</sup> ; any<sup>left?</sup> mov<sup>e y:(r6+CH808 _DECAY _TIME),a; nr of frames left in decay</sup> ; no,<sup>jump to fadeout</sup> tst<sup>a</sup> beq<sup>ch808_fadeout</sup> su<sup>b#>l,a</sup> nop mov<sup>e a,y:(r6+CH808_DECAY _TIME)</sup> bra<sup>ch808 _ decayout</sup> ·<sup>··</sup> ·<sup>·</sup> ·<sup>························••""'</sup> ,,,,,,<sup>,,,, ,,,,,,,,,,,,,,, ,,,,,,, ,,,,,</sup> ch80<sup>8_holdout</sup> ; n<sup>ote correct amplitude value is alreadey stored in b</sup> move<sup>b,yl</sup> do #<sup>I 6,plugin_ch808Loop_hold</sup> ; m<sup>ove noisefrom free_ x to yO</sup> ; mo<sup>ve noise from free_ x to xO</sup> mov<sup>e x:(r5)+,y0</sup> mov<sup>e x:(rS)+,xO</sup> ; th<sup>is is not needed ifwedeside to set volume to one at the start</sup> mpy<sup>yO,yl,a</sup> ; t<sup>his is not needed ifwedeside to set volumeto one at the start</sup> mpy<sup>xO,yl,b</sup> asr<sup>a</sup> asr b mov<sup>e a,y:(r7)+</sup> plugin<sup>_ ch808Loop _hold</sup> ; r<sup>estore vol to b</sup> mov<sup>eyl,b</sup> ·············<sup>·····</sup> •••••••••••••<sup>•</sup> •<sup>••••</sup> , , ''"<sup>,,,, ,,, ,,,,, ,,, , ,,,,, ,, ,,, ",,,</sup> ch808<sup>_decayout</sup> move<sup>y:(r6+CH808_DECAY _AMOUNT),xO</sup> ; for<sup>first mpy in loop</sup> move<sup>b,y1</sup> do #<sup>32,plugin_ch808Loop_decay</sup> ; u<sup>pdate amplitude envelope</sup> ; m<sup>ovenoisefrom free_xto yO</sup> sub<sup>xO,b</sup> mov<sup>e x:(rS)+,yO</sup> ; n<sup>ote correct amplitude value is alreadey stored in b</sup> mpy y<sup>O,yl,a</sup> oe asr sr<sup>a</sup> ee **30 Dlg;tal**<sup>**trnmsyntes TRX 10, ElekHon r,lach;ned,um SPS•1 EX06412004**</sup> 

Seoe asr sr<sup>a</sup> ee **Dlg;tal** 



move b,yl move a,y:(r7)+ ; save envelope rts ch808<sup>fadeout</sup> ; for first mpy in loop move<sup>b,yl</sup> do #32,<sup>plugin_ch808Loop_fade</sup> mpy<sup>x0,yl,b</sup> asr a move<sup>b,yl</sup> plugin<sup>_ ch808Loop _fade</sup> move<sup>b,y:(r6+CH808_AMP _ENV)</sup> rts **Digital**<sup>**trumsyntes TRX tor ElektronMachinedrum SPS-1 EX064/2004**</sup> **31** a ; note correct amplitude value is alreadey stored in b move a,y:(r7)+ ; move noise from free_ x to y0 **Appendix B** ; save envelope mpy y0,yl,a move y:(r6+CH808 _FADE _AMOUNT),xO move b,y:(r6+CH808_AMP _ENV) 

