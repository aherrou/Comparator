import("stdfaust.lib");

process = no.noise;
	  // : + ~ _;
	  // : moyenne;

moyenne(x) = (x + x')/2;
