#### Format data to insert in the STM model
#### By Steve Vissault
#### October 6th, 2014

################### DOM PARAMETERS

# Setup wd, source files, libraries -----------------------------------------------
  
  setwd("../../")

  # Load file ---------------------------------------------------------------
  parsfile="dompars3.txt"
  pars  <- read.table(parsfile,sep="",header = F)
  names(pars)  <- c("pars","value")
  pars_name  <- as.character(pars$pars)
  pars_value  <- pars$value
  
  for (i in 1:length(pars_name)) assign(pars_name[i],pars_value[i])
  
  ENV1 <- seq(-4,6,0.1)
  ENV2  <- 1.5
  
  logit_alphab = ab0 + ab1*ENV1 + ab2*ENV2 + ab3*ENV1^2 + ab4*ENV2^2 + ab5*ENV1^3 + ab6*ENV2^3
  logit_alphat = at0 + at1*ENV1 + at2*ENV2 + at3*ENV1^2 + at4*ENV2^2 + at5*ENV1^3 + at6*ENV2^3
  logit_betab = bb0 + bb1*ENV1 + bb2*ENV2 + bb3*ENV1^2 + bb4*ENV2^2 + bb5*ENV1^3 + bb6*ENV2^3
  logit_betat = bt0 + bt1*ENV1 + bt2*ENV2 + bt3*ENV1^2 + bt4*ENV2^2 + bt5*ENV1^3 + bt6*ENV2^3
  logit_thetab = tb0 + tb1*ENV1 + tb2*ENV2 + tb3*ENV1^2 + tb4*ENV2^2 + tb5*ENV1^3 + tb6*ENV2^3
  logit_thetat = tt0 + tt1*ENV1 + tt2*ENV2 + tt3*ENV1^2 + tt4*ENV2^2 + tt5*ENV1^3 + tt6*ENV2^3
  logit_eps = e0 + e1*ENV1 + e2*ENV2 + e3*ENV1^2 + e4*ENV2^2 + e5*ENV1^3 + e6*ENV2^3
  
  alphab = exp(logit_alphab)/(1+exp(logit_alphab))
  alphat = exp(logit_alphat)/(1+exp(logit_alphat))
  betab = exp(logit_betab)/(1+exp(logit_betab))
  betat = exp(logit_betat)/(1+exp(logit_betat))
  thetab = exp(logit_thetab)/(1+exp(logit_thetab))
  thetat = exp(logit_thetat)/(1+exp(logit_thetat))
  eps = exp(logit_eps)/(1+exp(logit_eps))
  
 
plot(ENV1,eps,type="l")
plot(ENV1,alphab,type="l")
plot(ENV1,alphat,type="l")
plot(ENV1,betab,type="l")
plot(ENV1,betat,type="l")
plot(ENV1,thetab,type="l")
plot(ENV1,thetat,type="l")
