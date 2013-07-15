##############################################################
#       Add quantile to table
##############################################################
reduce2quantils <- function(table,lower_quantil, upper_quantil){

	maximum <- max(table)
	for(i in 1:dim(table)[1]){
		for(j in 1:dim(table)[2]){
			if(table[i,j] < lower_quantil){
				table[i,j] <- 0
			}
			else if(table[i,j] > upper_quantil){
				table[i,j] <- (upper_quantil+(upper_quantil/18))
			}
		}
	}
	return(table)
}

################################################################################
#   Print table with legend considering quantile and adapted or fixed colors
################################################################################
print_table <- function(table, sender_region, receiver_region, col, mode, size, legend_pos, global_min, global_max, quantil){

	quantil_colors <- c(rgb(0,0,0),rgb(0.142,0.000,0.850),rgb(0.097,0.112,0.970),rgb(0.160,0.342,1.000),rgb(0.240,0.531,1.000),rgb(0.340,0.692,1.000),rgb(0.460,0.829,1.000),rgb(0.600,0.920,1.000),rgb(0.740,0.978,1.000),rgb(0.920,1.000,1.000),rgb(1.000,1.000,0.920),rgb(1.000,0.948,0.740),rgb(1.000,0.840,0.600),rgb(1.000,0.676,0.460),rgb(1.000,0.472,0.340),rgb(1.000,0.240,0.240),rgb(0.970,0.155,0.210),rgb(0.850,0.085,0.187),rgb(0.650,0.000,0.130),rgb(1,1,0))


	colors <- c(rgb(0.142,0.000,0.850),rgb(0.097,0.112,0.970),rgb(0.160,0.342,1.000),rgb(0.240,0.531,1.000),rgb(0.340,0.692,1.000),rgb(0.460,0.829,1.000),rgb(0.600,0.920,1.000),rgb(0.740,0.978,1.000),rgb(0.920,1.000,1.000),rgb(1.000,1.000,0.920),rgb(1.000,0.948,0.740),rgb(1.000,0.840,0.600),rgb(1.000,0.676,0.460),rgb(1.000,0.472,0.340),rgb(1.000,0.240,0.240),rgb(0.970,0.155,0.210),rgb(0.850,0.085,0.187),rgb(0.650,0.000,0.130))

#	pdf("test.pdf")

#Text for labels
	xlabel <- paste("Receiver")
	ylabel <- paste("Sender")


#Text for lower and upper quantil in legend
	text_min <- paste("< ", quantil, "% Quantil")
	text_max <- paste("> ", 100-quantil, "% Quantil")

#Adapted Colors
	if(col=="adapted"){

		if(quantil != 0){
			lower_quantil <- quantile(table,probs=(quantil/100))
			upper_quantil <- quantile(table,probs=((100-quantil)/100))
			table <- reduce2quantils(table,lower_quantil, upper_quantil)
			temp_colors <- quantil_colors
		}
		else{
			temp_colors <- colors
		}

		table_x <- dim(table)[1]
		table_y <- dim(table)[2]

		image(1:table_x, 1:table_y, table, col=temp_colors, xlab=xlabel, ylab=ylabel, axes=FALSE)
		tickmarks_2_at <- c(1,table_y/4, table_y/2, (table_y*3)/4, table_y)
		tickmarks_1_at <- c(1,table_x/4, table_x/2, (table_x*3)/4, table_x)
		tickmarks_1_label <- c(sender_region[1],sender_region[length(sender_region)/4],sender_region[length(sender_region)/2],sender_region[(length(sender_region)*3)/4],sender_region[length(sender_region)])
		tickmarks_2_label <- c(receiver_region[1],receiver_region[length(receiver_region)/4],receiver_region[length(receiver_region)/2],receiver_region[(length(receiver_region)*3)/4],receiver_region[length(receiver_region)])
		axis(2,tick=TRUE, labels=tickmarks_2_label, at=tickmarks_2_at)
		axis(1,tick=TRUE, labels=tickmarks_1_label, at=tickmarks_1_at)
		if(mode=="count"){

			title("Count", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, round(lower_quantil), " ", round(upper_quantil/2), " ", round(upper_quantil), text_max), fill=c(quantil_colors[1], quantil_colors[2], quantil_colors[length(quantil_colors)/4], quantil_colors[length(quantil_colors)/2], quantil_colors[(3*length(quantil_colors))/4], quantil_colors[length(quantil_colors)-1], quantil_colors[length(quantil_colors)]), bg="white",box.lwd=0, bty="o", title="'num of msgs'")
			}
			else {
				legend(x=legend_pos, legend=c(round(min(table)), " ", round(max(table)/2), " ", round(max(table))), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'num of msgs'")
			}
		}
		else if(mode=="length"){
			title("Length", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, round(lower_quantil), " ", round(upper_quantil/2), " ", round(upper_quantil), text_max), fill=c(quantil_colors[1], quantil_colors[2], quantil_colors[length(quantil_colors)/4], quantil_colors[length(quantil_colors)/2], quantil_colors[(3*length(quantil_colors))/4], quantil_colors[length(quantil_colors)-1], quantil_colors[length(quantil_colors)]), bg="white",box.lwd=0, bty="o", title="'bytes'")
			}
			else{
				legend(x=legend_pos, legend=c(round(min(table)), " ", round(max(table)/2), " ", round(max(table))), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'bytes'")
			}
		}
		else if(mode=="duration"){
			title("Duration", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, round(lower_quantil, digits=5), " ", round(upper_quantil/2, digits=5), " ", round(upper_quantil, digits=5), text_max), fill=c(quantil_colors[1], quantil_colors[2], quantil_colors[length(quantil_colors)/4], quantil_colors[length(quantil_colors)/2], quantil_colors[(3*length(quantil_colors))/4], quantil_colors[length(quantil_colors)-1], quantil_colors[length(quantil_colors)]), bg="white",box.lwd=0, bty="o", title="'seconds'")
			}
			else{
				legend(x=legend_pos, legend=c(round(min(table),5), " ", round(max(table)/2,5), " ", round(max(table),5)), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'seconds'")
			}
		}
		else if(mode=="rate"){
			title("Rate", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, format(lower_quantil, scientific=TRUE, digits=3), " ", format(upper_quantil/2,scientific=TRUE,digits=3), " ", format(upper_quantil,scientific=TRUE,digits=3), text_max), fill=c(quantil_colors[1], quantil_colors[2], quantil_colors[length(quantil_colors)/4], quantil_colors[length(quantil_colors)/2], quantil_colors[(3*length(quantil_colors))/4], quantil_colors[length(quantil_colors)-1], quantil_colors[length(quantil_colors)]), bg="white",box.lwd=0, bty="o", title="'byte/second'")
			}
			else {
				legend(x=legend_pos, legend=c(round(min(table)), " ", format(max(table)/2, scientific=TRUE, digits=3), " ", format(max(table), scientific=TRUE, digits=3)), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'bytes/second'")
			}
		}
	}

#Fixed Colors
	else if(col=="fixed"){
		diff_factor <- round((global_max/max(table)))
		col_max <- round((length(colors)/diff_factor))
		if(col_max==1){ col_max=2 }

		if(quantil == 0){
			temp_colors <- c(colors[1:col_max])
		}
		else{
			lower_quantil <- quantile(c(global_min, global_max),probs=(quantil/100))
			upper_quantil <- quantile(c(global_min, global_max),probs=((100-quantil)/100))
			temp_colors <- c(rgb(0,0,0),colors[1:col_max],rgb(1,1,0))
			table <- reduce2quantils(table,lower_quantil, upper_quantil)
		}

		image(1:dim(table)[1], 1:dim(table)[2], table, col=temp_colors, xlab=xlabel, ylab=ylabel, axes=FALSE)
		table_x <- dim(table)[1]
		table_y <- dim(table)[2]
		tickmarks_2_at <- c(1,table_y/4, table_y/2, (table_y*3)/4, table_y)
		tickmarks_1_at <- c(1,table_x/4, table_x/2, (table_x*3)/4, table_x)
		tickmarks_1_label <- c(sender_region[1],sender_region[length(sender_region)/4],sender_region[length(sender_region)/2],sender_region[(length(sender_region)*3)/4],sender_region[length(sender_region)])
		tickmarks_2_label <- c(receiver_region[1],receiver_region[length(receiver_region)/4],receiver_region[length(receiver_region)/2],receiver_region[(length(receiver_region)*3)/4],receiver_region[length(receiver_region)])
		axis(2,tick=TRUE, labels=tickmarks_2_label, at=tickmarks_2_at)
		axis(1,tick=TRUE, labels=tickmarks_1_label, at=tickmarks_1_at)


		if(mode=="count"){
			title("Count", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, round(lower_quantil), " ", round(upper_quantil/2), " ", round(upper_quantil), text_max), fill=c(temp_colors[1], temp_colors[2], temp_colors[length(temp_colors)/4], temp_colors[length(temp_colors)/2], temp_colors[(3*length(temp_colors))/4], temp_colors[length(temp_colors)-1], temp_colors[length(temp_colors)]), bg="white",box.lwd=0, bty="o", title="'num of msgs'")
			}
			else{
				legend(x=legend_pos, legend=c(round(global_min), " ", round(global_max/2), " ", round(global_max)), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'num of msgs'")
			}
		}
		else if(mode=="length"){
			title("Length", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, round(lower_quantil), " ", round(upper_quantil/2), " ", round(upper_quantil), text_max), fill=c(temp_colors[1], temp_colors[2], temp_colors[length(temp_colors)/4], temp_colors[length(temp_colors)/2], temp_colors[(3*length(temp_colors))/4], temp_colors[length(temp_colors)-1], temp_colors[length(temp_colors)]), bg="white",box.lwd=0, bty="o", title="'bytes'")
			}
			else{
				legend(x=legend_pos, legend=c(round(global_min), " ", round(global_max/2), " ", round(global_max)), fill=c(colors[1], colors[length(colors)/4],colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'bytes'")
			}
		}
		else if(mode=="duration"){
			title("Duration", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, round(lower_quantil, digits=5), " ", round(upper_quantil/2, digits=5), " ", round(upper_quantil, digits=5), text_max), fill=c(temp_colors[1], temp_colors[2], temp_colors[length(temp_colors)/4], temp_colors[length(temp_colors)/2], temp_colors[(3*length(temp_colors))/4], temp_colors[length(temp_colors)-1], temp_colors[length(temp_colors)]), bg="white",box.lwd=0, bty="o", title="'seconds'")
			}
			else{
				legend(x=legend_pos, legend=c(round(global_min,5), " ", round(global_max/2,5), " ", round(global_max,5)), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'seconds'")
			}
		}
		else if(mode=="rate"){
			title("Rate", cex.main = 2,   font.main= 4, col.main= "black")
			if(quantil != 0){
				legend(x=legend_pos, legend=c(text_min, format(lower_quantil, scientific=TRUE, digits=3), " ", format(upper_quantil/2,scientific=TRUE,digits=3), " ", format(upper_quantil,scientific=TRUE,digits=3), text_max), fill=c(temp_colors[1], temp_colors[2], temp_colors[length(temp_colors)/4], temp_colors[length(temp_colors)/2], temp_colors[(3*length(temp_colors))/4], temp_colors[length(temp_colors)-1], temp_colors[length(temp_colors)]), bg="white",box.lwd=0, bty="o", title="'byte/second'")
			}
			else{
				legend(x=legend_pos, legend=c(round(global_min), " ", format(global_max/2, scientific=TRUE, digits=3), " ", format(global_max, scientific=TRUE, digits=3)), fill=c(colors[1], colors[length(colors)/4], colors[length(colors)/2], colors[(3*length(colors))/4], colors[length(colors)]), bg="white",box.lwd=0, bty="o", title="'bytes/second'")
			}
		}
	}
#	dev.off()
}




####################################################
#   Main function
####################################################
salsa <- function(path, shrink_mode="max", shrink_dim=512, sender_region=NULL, receiver_region=NULL, col="adapted", legend_pos="topleft", plot=TRUE, quantil=0){

	if(substr(path, nchar(path), nchar(path)) != "/"){
		path <- paste(path,"/",sep="")
	}
	metafile <- paste(path,"meta.data",sep="")

	metadata <- scan(metafile,what=list("",""),skip=1)


#Read Data from File
	size <- as.numeric(metadata[[2]][1])
	print(size)
	blocksize <- as.numeric(metadata[[2]][2])
	print(blocksize)
	mode <- metadata[[2]][3]
	global_min <- as.numeric(metadata[[2]][6])
	global_max <- as.numeric(metadata[[2]][7])

	blocknumber <- (size/blocksize)

#No shrinking if plot==FALSE
	if(plot==FALSE){
		shrink_dim <- size
	}


	if(length(sender_region)==0){
		sender_region <- 1:size
	}
	if(length(receiver_region)==0){
		receiver_region <- 1:size
	}

#Region of matrix to display
	x_min <- sender_region[1]
	x_max <- sender_region[length(sender_region)]
	y_min <- receiver_region[1]
	y_max <- receiver_region[length(receiver_region)]

#Region of matrix to read
	table_x_min <- ((((x_min-1)%/%blocksize)*blocksize)+1)
	table_x_max <- ((((x_max-1)%/%blocksize)+1)*blocksize)

#Blocks to read
	first_block <- (((table_x_min-1)%/%blocksize)+1)
	last_block <- (table_x_max%/%blocksize)

##create matrix
#	tmp_table <- matrix()
#	tmp_table <- matrix(0,size*blocksize)
#	tmp_table <- matrix(tmp_table, nrow=size)

#  #read values from file
#      for(i in 1:blocknumber){
#          if((i >= first_block) & (i <= last_block)){
#              datei <- paste(path,"block",(i-1)*blocksize,".out",sep="")
#              input_table <- read.table(datei)
#
#              if(sparse==FALSE){
#                  input_table <- as.matrix(input_table)
#                  input_table <- matrix(input_table,nrow=blocksize)
#                  print(input_table)
#              }
#  #Sparse Input
#              else if(sparse==TRUE){
#                  write(c(datei,size,blocksize,dim(input_table)[1],mode),"/home5/jzam11/jzam1104/diplom/r-files/tmp.txt")
#                  input_table <- read.table(pipe("/home5/jzam11/jzam1104/diplom/salsa/rebuild /home5/jzam11/jzam1104/diplom/r-files/tmp.txt"))
#                  input_table <- as.matrix(input_table)
#                  input_table <- matrix(input_table,nrow=blocksize)
#              }
#
#              print(paste("read block ", (i-first_block+1), "of ", (last_block-first_block+1)))
#
#  #dimensions of input_table and region
#              table_dim_x <- c(1,size)
#              table_dim_y <- c(1,blocksize)
#
#              if(i==1){
#                  ##x_start <- (x_min %% table_dim_x[1])+1
#                  x_start <- (x_min - (first_block-1)*table_dim_x[2])
#              }
#              else{
#                  x_start <- table_dim_x[1]
#              }
#              if(i==blocknumber){
#                  if((x_max %% table_dim_x[2]) == 0){
#                      x_end <- table_dim_x[2]
#                  }
#                  else{
#                      x_end <- (x_max %% table_dim_x[2])
#                  }
#              }
#              else{
#                  x_end <- table_dim_x[2]
#              }
#              print("Dims created!")
#  #if shrinking necessary
#              if(((x_max-x_min) > shrink_dim) | ((y_max-y_min) > shrink_dim)){
#
#                  block_table <- shrink_matrix(input_table[y_min:y_max,x_start:x_end], x_max, x_min, x_end, x_start, shrink_dim, shrink_mode)
#
#              }
#              else{
#                  block_table <- input_table[y_min:y_max,x_start:x_end]
#              }
#
#  #create table with first block
#  ##             if(i == first_block){
#                  print("create first!")
#                  table <- block_table
#
#              }
#
#  #add blocks to table
#              else {
#                  print("create other!")
#                  table <- cbind(table,block_table)
#
#              }
#          }
#      }
	#datei <- paste(path,"block",(i-1)*blocksize,".out",sep="")
	write(c(path,shrink_mode,shrink_dim),"tmp.txt")
    table <- read.table(pipe("salsa_rebuild tmp.txt"))
	#file.remove("tmp.txt")
    table <- as.matrix(table)
	if(shrink_dim < size){
		size <- shrink_dim
	}
    table <- matrix(table,nrow=size)
	if(plot==TRUE){
#print region of table
		print_table(table, y_min:y_max, x_min:x_max, col, mode, size, legend_pos, global_min, global_max, quantil)
	}
	else{
		return(table)
	}
}

