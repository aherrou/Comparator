import("stdfaust.lib");

// Karplus Strong (1/2)
process = // no.noise*(1-1@200) :
  ba.pulse(10000) : // moyenne;
  // 1 - 1' :
  + ~ transformation;

transformation = @(50) : moyenne ; //: *(0.9375);

moyenne(x) = (x+x')/2.125;
