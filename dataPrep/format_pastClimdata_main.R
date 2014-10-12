#### Format data to insert in the STM model
#### By Steve Vissault
#### October 6th, 2014

rm(list=ls())

# Setup wd, source files, libraries -----------------------------------------------

setwd("~/Documents/GitHub/C-STMFor/")
source("dataPrep/extract_int_yrs_pastClimate.R")
library(ggplot2)
library(reshape2)
library(ggmap)
library(plyr)
library(RColorBrewer)
library(gstat)

# Setup paths and variables -------------------------------------------------------------

path_annual_mean_temp  <- "~/Documents/Data/Raster_pastClimate_STM/ascii/bio300_01/"
path_annual_pp   <- "~/Documents/Data/Raster_pastClimate_STM/ascii/bio300_12/"
yearMin  <- 1970
yearMax  <- 2000

# extract variables on crop area for all years-------------------------------------------------------

rs.annual_mean_temp  <- get_bioclim_var(path_annual_mean_temp,yearMin,yearMax)
rs.annual_pp  <- get_bioclim_var(path_annual_pp,yearMin,yearMax)

# Compute average on all layers of the rasterstack ------------------------

rs_avg.annual_mean_temp  <- mean(rs.annual_mean_temp)
rs_avg.annual_pp  <- mean(rs.annual_pp)

# Switch to dataframe structure ----------------------------------------------

df.annual_mean_temp  <- as.data.frame(rs_avg.annual_mean_temp,na.rm=FALSE,xy=TRUE)
df.annual_pp  <- as.data.frame(rs_avg.annual_pp,na.rm=FALSE,xy=TRUE)

# Final dataset with units (annual_pp (m) and annual_temp (C))-----------------------------------------------------------

clim_var  <- data.frame(lon = df.annual_mean_temp$x, lat = df.annual_mean_temp$y, avg_annual_temp = df.annual_mean_temp$layer/10, avg_annual_pp = df.annual_pp$layer/1000)

# Visualization -----------------------------------------------------------

ggplot(aes(x = lon, y = lat), data = clim_var) + geom_tile(aes(fill = avg_annual_temp)) + coord_equal()
ggplot(aes(x = lon, y = lat), data = clim_var) + geom_tile(aes(fill = avg_annual_pp)) + coord_equal()

# Transfo to STM format ---------------------------------------------------

# Transform lat and lon as factors
clim_var$f.lon  <- as.factor(clim_var$lon)
clim_var$f.lat  <- as.factor(clim_var$lat) # levels of the factor is ordered ACS

# function to get coord based on the STM model (using split and factors)
get_grid_coord  <- function(x,get_mean_NA=TRUE){
  x= clim_var
  ls_clim_var_STMCoord  <- split(x,x$f.lon)
  
  coordx  <- seq(0,length(ls_clim_var_STMCoord)-1,1)
  
    for (i in 1: length(ls_clim_var_STMCoord)){
      ls_clim_var_STMCoord[[i]]$x  <- rep(coordx[i],nrow(ls_clim_var_STMCoord[[i]]))
    }

  ls_clim_var_STMCoord <- do.call("rbind", ls_clim_var_STMCoord) 
  
  ls_clim_var_STMCoord  <- split(ls_clim_var_STMCoord,ls_clim_var_STMCoord$f.lat)
  coordy  <- seq(0,length(ls_clim_var_STMCoord)-1,1)
  
    for (i in 1: length(ls_clim_var_STMCoord)){
      ls_clim_var_STMCoord[[i]]$y  <- rep(coordy[i],nrow(ls_clim_var_STMCoord[[i]]))
      if(get_mean_NA == TRUE) {
        ls_clim_var_STMCoord[[i]][is.na(ls_clim_var_STMCoord[[i]]$avg_annual_temp),"avg_annual_temp"] = mean(ls_clim_var_STMCoord[[i]]$avg_annual_temp,na.rm=TRUE)
        ls_clim_var_STMCoord[[i]][is.na(ls_clim_var_STMCoord[[i]]$avg_annual_pp),"avg_annual_pp"] = mean(ls_clim_var_STMCoord[[i]]$avg_annual_pp,na.rm=TRUE)
      }
    }
    
  ls_clim_var_STMCoord <- do.call("rbind", ls_clim_var_STMCoord) 
  
  return(ls_clim_var_STMCoord)
}

# get Coords and reshape for the outpu
clim_var <- get_grid_coord(clim_var)

# Create idx with geographic coordinates
STM_idx  <- data.frame(clim_var[1:2],clim_var[7:8])
write.table(STM_idx, file="./coord_idx.csv", sep=',', row.names=FALSE)

#Write ouput
STM_climData  <- data.frame(x= clim_var[,7],y= clim_var[,8],year=rep(0,nrow(clim_var)),env1=clim_var[,3],env2=clim_var[,4])
write.table(STM_climData, file="./PastClimate70-00.csv", sep=',', row.names=FALSE)
