### Compute proportion of each step 
### By Steve Vissault
### October, 12th 2014

rm(list=ls())

# Set wd and libraries------------------------------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/outDir/brick_out/")
library(raster)
library(stringr)

# Functions ---------------------------------------------------------------
compute_prop_layer  <- function(layer,brick){
  pos=which(names(brick)==layer)
  m_simu<-as.matrix(brick[[pos]])
  #Compute prop
  prop_R<-length(which(m_simu==0))/length(m_simu)
  prop_T<-length(which(m_simu==1))/length(m_simu)
  prop_B<-length(which(m_simu==2))/length(m_simu)
  prop_M<-length(which(m_simu==3))/length(m_simu)
  
  out_df  <- data.frame(step=as.factor(str_extract(layer,"[0-9]+")),prop_R=prop_R,prop_T=prop_T,prop_B=prop_B,prop_M=prop_M)
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



