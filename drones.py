import math

file = open("busy_day.in","rw+")

[r,c,Drones,Deadline,MaxPayload] = file.readline().split( )
ProductTypes = file.readline().split( )
ProductsWeight = file.readline().split( )
WarehousesNum = int(file.readline())
WareHouse = []
Deadline=int(Deadline)
for i in range (0,WarehousesNum):
	WareHouse.append([0,0,[]])
	#print(WareHouse)
	#print("*******")
	[WareHouse[i][0],WareHouse[i][1]] = file.readline().split( )

	for j in file.readline().split( ):
		WareHouse[i][2].append(j) 
	#print(WareHouse)
	#print("*******")

DronesLoc = []
Drones = int(Drones)
for i in range(0,Drones):
	DronesLoc.append([0,0,0])

NumberOfOrders = int(file.readline())
Delev = []
for i in range (0,NumberOfOrders):
	Delev.append([0,0,0,[]])
	[Delev[i][0],Delev[i][1]] = file.readline().split( )
	Delev[i][2] = int(file.readline())
	for j in file.readline().split( ):
		Delev[i][3].append(j)

file.close()

file = open("greedy_busy.out","rw+")
##Validator
NumberOfCommands = file.readline().split( )
if len(NumberOfCommands)!=1:
	print("Invalid Number of Commands",len(NumberOfCommands))
	quit()

NumberOfCommands = int(NumberOfCommands[0])
NumberOfLines = 0;
line = file.readline()
while line:
	ParseLine = line.split( )
	if ParseLine[1] in ['L','U']:
		if len(ParseLine) != 5:
			print("Wrong Number of argument, expecting 5", ParseLine)
		Dx=int(DronesLoc[int(ParseLine[0])][0])
		Dy= int(DronesLoc[int(ParseLine[0])][1])
		Wx=int(WareHouse[int(ParseLine[2])][0])
		Wy=int(WareHouse[int(ParseLine[2])][1])
		CalcDest = math.sqrt(math.pow(Dx-Wx,2)+math.pow(Dy-Wy,2))
		DronesLoc[int(ParseLine[0])][2] = DronesLoc[int(ParseLine[0])][2] + CalcDest +1
		DronesLoc[int(ParseLine[0])][0] = int(WareHouse[int(ParseLine[2])][0])
		DronesLoc[int(ParseLine[0])][1] = int(WareHouse[int(ParseLine[2])][1])
	elif ParseLine[1]=='D':
		if len(ParseLine) != 5:
			print("Wrong Number of argument, expecting 5", ParseLine)	
		Dx=int(DronesLoc[int(ParseLine[0])][0])
		Dy= int(DronesLoc[int(ParseLine[0])][1])
		Cx=int(Delev[int(ParseLine[2])][0])
		Cy=int(Delev[int(ParseLine[2])][1])
		CalcDest = math.sqrt(math.pow(Dx-Cx,2)+math.pow(Dy-Cy,2))
		DronesLoc[int(ParseLine[0])][2] = DronesLoc[int(ParseLine[0])][2] + CalcDest +1
		DronesLoc[int(ParseLine[0])][0] = int(Delev[int(ParseLine[2])][0])
		DronesLoc[int(ParseLine[0])][1] = int(Delev[int(ParseLine[2])][1])
	elif ParseLine[1] == 'W':
		if len(ParseLine) != 3:
			print("Wrong Number of argument, expecting 3", ParseLine)
		Drones[int(ParseLine[0])][2] = Drones[int(ParseLine[0])][2] + int(ParseLine[2])
	else:
		print("Unknown Command",ParseLine)
		quit()
	line = file.readline()

for i in range(0,Drones):
	print("Scoring for Drone",i, "is:",100*(Deadline - int(DronesLoc[i][2]))/Deadline)