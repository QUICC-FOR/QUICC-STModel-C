### Prepare future climate scenarios
### By Steve Vissault
### October, 14th 2014

rm(list=ls())

setwd("~/Documents/GitHub/C-STMFor/")

pastClimate  <- read.table("PastClimate70-00.csv",sep=",",header=TRUE)

inc_clim  <- 4/9

ls_futureClim  <- list()

for(i in 1:10){
  if(i==1){
    yr  <- pastClimate
    ls_futureClim[[i]]  <- yr
  } else {
    yr  <- ls_futureClim[[i-1]]
    yr$year  <- yr$year+1
    yr$env1  <- yr$env1 + inc_clim
    ls_futureClim[[i]]  <- yr 
  }
}
  

futureClim  <- do.call(rbind,ls_futureClim)
write.table(futureClim, file="./futureClimate100.csv", sep=',', row.names=FALSE)
