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


# Find clim var space for NA value ----------------------------------------

temp <- seq(range(clim_var$avg_annual_temp)[1],range(clim_var$avg_annual_temp)[2],0.01)
pp <- seq(range(clim_var$avg_annual_pp)[1],range(clim_var$avg_annual_pp)[2],0.01)

clim_space  <- expand.grid(temp,pp)
names(clim_space)  <- c("temp","pp")

ENV1 = clim_space$temp
ENV2 = clim_space$pp

### epsilon

e0 = -5.02835512140436
e1 = -0.896555741457188
e3 = 0.110811540916957
e2 = 10.0169100493245
e4 = -5.77001585627923
e5 = 0
e6 = 0

logit_eps = e0 + e1*ENV1 + e2*ENV2 + e3*ENV1^2 + e4*ENV2^2 + e5*ENV1^3 + e6*ENV2^3
eps = exp(logit_eps)/(1+exp(logit_eps))

### alpha_b

ab0 = -1.16361552347762
ab1 = -0.123487321020654
ab3 = 0.0611134973090761
ab2 = 1.36258369833543
ab4 = 0.00617230638848189
ab5 = 0
ab6 = 0

logit_alphab = ab0 + ab1*ENV1 + ab2*ENV2 + ab3*ENV1^2 + ab4*ENV2^2 + ab5*ENV1^3 + ab6*ENV2^3
alphab = exp(logit_alphab)/(1+exp(logit_alphab))

### alpha_t

at0 = -4.25796438158399
at1 = 0.0116538718099561
at3 = 0.0537659679607981
at2 = -1.11055959620349
at4 = 4.42399157755706
at5 = 0
at6 = 0

logit_alphat = at0 + at1*ENV1 + at2*ENV2 + at3*ENV1^2 + at4*ENV2^2 + at5*ENV1^3 + at6*ENV2^3
alphat = exp(logit_alphat)/(1+exp(logit_alphat))

# Final dataset
dat  <- cbind(clim_space,eps,alphab,alphat)
ggdat  <- melt(dat, id.vars = c("pp","temp"),value.name = "prob",na.rm = TRUE)
ggdat$prob  <- cut(ggdat$prob,10)

qplot(pp, temp, data = ggdat, fill = prob, geom = "raster")  + facet_grid(~ variable) + 
  scale_fill_manual(values = rev(brewer.pal(10,"RdBu"))) + 
  xlab("Precipitation (m)") +
  ylab("Temperature (°C)")


plot(x=dat$alphab,y=ENV1)
#sub  <- dat[which(dat$pp>-1&dat$pp<0),]
plot(ENV1,dat$alphat)




