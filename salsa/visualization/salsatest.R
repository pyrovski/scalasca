args=(commandArgs(TRUE))
if(length(args)==0){
	print("Error! No arguments supplied.")
	print("At least the Path to the SALSA archive must be supplied!")
	##supply default values
}else{
	for(i in 1:length(args)){
		eval(parse(text=args[[i]]))
	}
}

source("salsa.R")
pdf("salsatest.pdf")
salsa("/home/knobi/programming/c/epik_ring_dyn_4_trace__1_count")
dev.off()
