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

# Setup paths and variables -------------------------------------------------------------

path_annual_mean_temp  <- "~/Documents/Data/Raster_pastClimate_STM/ascii/bio300_01/"
path_annual_pp   <- "~/Documents/Data/Raster_pastClimate_STM/ascii/bio300_12/"
yearMin  <- 1970
yearMax  <- 2000

# extract variables -------------------------------------------------------

rs.annual_mean_temp  <- get_bioclim_var(path_annual_mean_temp,yearMin,yearMax)
rs.annual_pp  <- get_bioclim_var(path_annual_pp,yearMin,yearMax)

# Get mean on years interval ----------------------------------------------

df.annual_mean_temp  <- as.data.frame(mean(rs.annual_mean_temp),na.rm=FALSE,xy=TRUE)
df.annual_pp  <- as.data.frame(mean(rs.annual_pp),na.rm=FALSE,xy=TRUE)

# Final dataset with units (annual_pp (m) and annual_temp (C))-----------------------------------------------------------

clim_var  <- data.frame(lon = df.annual_mean_temp$x, lat = df.annual_mean_temp$y, avg_annual_temp = df.annual_mean$layer/10, avg_annual_pp = df.annual_pp$layer/1000)

# Visualization -----------------------------------------------------------

ggplot(aes(x = lon, y = lat), data = clim_var) + geom_tile(aes(fill = avg_annual_temp)) + coord_equal()
ggplot(aes(x = lon, y = lat), data = clim_var) + geom_tile(aes(fill = avg_annual_pp)) + coord_equal()

# Transfo to STM format ---------------------------------------------------

# transform lat and lon as factors
clim_var$lon  <- as.factor(clim_var$lon)
clim_var$lat  <- as.factor(clim_var$lat) # levels of the factor is ordered ACS


ls_clim_var  <- split(clim_var,clim_var$lon)






