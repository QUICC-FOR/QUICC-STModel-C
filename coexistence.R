rm(list = ls())
setwd("/Users/DGravel/Documents/Projects_On_Going/Maple_migration/transition_maple/analyses/v3")
attach(read.table("par.txt"))
par  = read.table("par.txt")

ENV = expand.grid(TP = seq(-1,6,0.01), PP = seq(0.9,1.5,0.001))
TP = ENV$TP
PP = ENV$PP

# Compute the logit
logit_alphad 	= ad0 + adt1*TP + adt2*TP^2 + adp1*PP + adp2*PP^2
logit_alphac 	= ac0 + act1*TP + act2*TP^2 + acp1*PP + acp2*PP^2
logit_betad		= bd0 + bdt1*TP + bdt2*TP^2 + bdp1*PP + bdp2*PP^2
logit_betac		= bc0 + bct1*TP + bct2*TP^2 + bcp1*PP + bcp2*PP^2
logit_thetad	= td0 + tdt1*TP + tdt2*TP^2 + tdp1*PP + tdp2*PP^2
logit_thetac	= tc0 + tct1*TP + tct2*TP^2 + tcp1*PP + tcp2*PP^2
logit_eps 		= e0 + et1*TP + et2*TP^2 + ep1*PP + ep2*PP^2

# Back transform into probabilities
aC = exp(logit_alphac)/(1+exp(logit_alphac))
aD = exp(logit_alphad)/(1+exp(logit_alphad))
bC = exp(logit_betac)/(1+exp(logit_betac))
bD = exp(logit_betad)/(1+exp(logit_betad))
sC = exp(logit_thetac)/(1+exp(logit_thetac))
sD = exp(logit_thetad)/(1+exp(logit_thetad))
e = exp(logit_eps)/(1+exp(logit_eps))

# Compute the first eigenvalues for C and D as invaders
invC = 1/2*((aD - e)*bC - (aD - e)*bD + aC*e - 2*aD*e - aD*sC - aD*sD + sqrt(aC^2*e^2 + aD^2*sC^2 + aD^2*sD^2 + (aD^2 - 2*aD*e + e^2)*bC^2 + (aD^2 - 2*aD*e + e^2)*bD^2 + 2*(aC*aD*e - aC*e^2)*bC + 2*(aC*aD*e - aC*e^2 + (aD^2 - 2*aD*e + e^2)*bC)*bD + 2*(aC*aD*e + (aD^2 - aD*e)*bC + (aD^2 - aD*e)*bD)*sC - 2*(2*aC*aD^2*e - (2*e^2 + e)*aC*aD - aD^2*sC + (aD^2 - aD*e)*bC + (aD^2 - aD*e)*bD)*sD))/aD

invD = -1/2*((aC - e)*bC - (aC - e)*bD + 2*aC*e - aD*e + aC*sC + aC*sD - sqrt(aD^2*e^2 + aC^2*sC^2 + aC^2*sD^2 + 2*(aC*e - e^2)*aD*bC + (aC^2 - 2*aC*e + e^2)*bC^2 + (aC^2 - 2*aC*e + e^2)*bD^2 + 2*((aC*e - e^2)*aD + (aC^2 - 2*aC*e + e^2)*bC)*bD - 2*((2*aC^2*e - (2*e^2 + e)*aC)*aD + (aC^2 - aC*e)*bC + (aC^2 - aC*e)*bD)*sC + 2*(aC*aD*e + aC^2*sC + (aC^2 - aC*e)*bC + (aC^2 - aC*e)*bD)*sD))/aC

# Interpret the invasability criterion
coexist = numeric(length(invC))

# Reciprocal resistance (alternative stable states)
coexist[invC<0 & invD<0] = 0

# Species C wins
coexist[invC>0 & invD<0] = 1

# Species D winds
coexist[invC<0 & invD>0] = 2

# Reciprocal invasibility
coexist[invC > 0 & invD > 0] = 3

# Plot the results
X = seq(-1,6,0.01)
Y = seq(0.9,1.5,0.001)
Z = matrix(coexist,nr = length(X), nc = length(Y))
quartz(width = 6, height = 6)
layout(matrix(c(1,2),nr=2,nc=1,byrow=TRUE),heights = c(1,6))
par(mar=c(0,0,0,0))
plot(1, type = "n", axes=FALSE, xlab="", ylab="")
legend("center",legend = c("Boréal","Tempéré","Mixte"),fill = c("darkcyan","orange","palegreen3"),bty = "n",horiz = TRUE,cex = 1)
par(mar=c(5,5,0,2))
image(X,Y*1000,Z,xlab = "Température moyenne annuelle", ylab = "Précipitations annuelles (mm)", cex.lab = 1.5, cex.axis = 1.25, col = c("darkcyan","orange","palegreen3"))

dev.copy2pdf(file = "figures/Coexistence_area.pdf")




