import("stdfaust.lib");

phasor_imp(freq, reset, phase) = (select2(reset, +(freq/ma.SR), phase) : ma.decimal) ~ _;

// Version to be used with tables
phasor_table(tablesize, freq, reset, phase) = phasor_imp(freq, reset, phase) : *(float(tablesize));


phasor(tablesize, freq) = phasor_table(tablesize, freq, 0, 0);



process = phasor(hslider("table",0.5,0.1,1,0.1),hslider("freq",440,20,3000,1));