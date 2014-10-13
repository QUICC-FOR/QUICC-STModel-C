### Compute proportion of each step 
### By Steve Vissault
### October, 12th 2014

rm(list=ls())

# Set libraries------------------------------------------------------------------
library(raster)
library(stringr)
library(reshape2)
library(ggplot2)
library(plyr)
library(gridExtra)

# Info on simulation  ----------------------------------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/Times100-Rep5-IsaParsm3/")

nSimu <- 5
WritingSteps  <- 2
MaxTimeSteps  <- 100
y_Gridsize  <- 1600 
x_Gridsize  <- 500
all_steps  <- seq(WritingSteps,MaxTimeSteps,WritingSteps)
simu_paths  <- list.dirs(getwd())[str_detect(list.dirs(getwd()),"simu")]
ext  <- extent(-75,-70, 44.990798, 57.5)
proj = CRS("+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs +towgs84=0,0,0")


# Import to rasterbrick ---------------------------------------------------
## Need to be done one times
#source("../dataAnalyze/import_simu.R")

# Analyse prop ------------------------------------------------------------
setwd("./brick_out/")

# Functions ---------------------------------------------------------------
compute_prop_layer  <- function(layer,brick){
  pos=which(names(brick)==layer)
  m_simu<-as.matrix(brick[[pos]])
  #Compute prop
  prop_R<-length(which(m_simu==0))/length(m_simu)
  prop_T<-length(which(m_simu==1))/length(m_simu)
  prop_B<-length(which(m_simu==2))/length(m_simu)
  prop_M<-length(which(m_simu==3))/length(m_simu)
  
  out_df  <- data.frame(step=as.numeric(str_extract(layer,"[0-9]+")),prop_R=prop_R,prop_T=prop_T,prop_B=prop_B,prop_M=prop_M)
}
compute_prop_all_simus  <- function(simu_files_order){
  n_simu  <- as.factor(str_extract(simu_files_order,"[0-9]+"))
  rs_simu  <- brick(simu_files_order)
  layers  <- names(rs_simu)
  prop_layers  <- lapply(layers,compute_prop_layer,brick=rs_simu)
  prop_layers  <- do.call(rbind,prop_layers)
  prop_layers  <- data.frame(simu=rep(n_simu,nrow(prop_layers)),prop_layers)
}

# Compute prop for all simus

simu_files  <- paste(getwd(),list.files(),sep="/")[str_detect(list.files(),"grd")]
simu_files_order  <- simu_files[sort(as.numeric(str_extract(simu_files,"[0-9]+")), index.return = TRUE)$ix]

prop_all_simu  <- lapply(simu_files_order,compute_prop_all_simus)
prop_all_simu  <- do.call(rbind,prop_all_simu)
write.csv2(prop_all_simu,file="../prop_all_simus.csv")

gg_prop  <- melt(prop_all_simu,id=c("simu","step"),variable.name = "prop")
ggplot(gg_prop) + geom_point(aes(x=step,y=value,group=simu,color=prop)) + xlab("Time") +ylab("Proportion (%)")
ggsave("../prop_time.pdf")
