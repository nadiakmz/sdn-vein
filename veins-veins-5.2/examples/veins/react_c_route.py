#!/usr/bin/python
# -*- coding: utf-8 -*-

import os
import shutil
import sys
import fileinput
import math
from bs4 import BeautifulSoup
from sys import platform

def create_folder(dir):
	if not os.path.exists(dir): 
		os.makedirs(dir, 0o755)

def generate_trip(dirWork, current_num_seed, n_vehicles, map_name, net_file_name):
	# os.system("/Applications/sumo-0.30.0/tools/randomTrips.py  -n "+net_file_name+" -s "+str(current_num_seed)+" -e "+str(n_vehicles)+" -o "+getStoreFolder(dirWork, map_name, current_num_seed, n_vehicles)+map_name+".trips.xml")
	os.system("/opt/homebrew/Cellar/sumo/1.10.0/share/sumo/tools/randomTrips.py  -n "+net_file_name+" -s "+str(current_num_seed)+" -e "+str(n_vehicles)+" -o "+getStoreFolder(dirWork, map_name, current_num_seed, n_vehicles)+map_name+".trips.xml")

def generate_route(dirWork, current_num_seed, n_vehicles, map_name, net_file_name):
	os.system("duarouter -n "+net_file_name+" --ignore-errors -t "+getStoreFolder(dirWork, map_name, current_num_seed, n_vehicles)+map_name+".trips.xml  -o "+getStoreFolder(dirWork, map_name, current_num_seed, n_vehicles)+map_name+".rou.xml")

def create_sumo(dirActual,algoritmo, map_name, n_ruas, start, duration):
	print (dirActual+map_name+"-"+str(n_ruas)+'-'+str(start)+'-'+str(duration)+'-'+algoritmo+".sumo.cfg")
	sumo_file = open(dirActual+map_name+"-"+algoritmo+".sumo.cfg", 'w')
	sumo_text = '<?xml version="1.0" encoding="ISO-8859-1"?>\n\n<configuration>\n\n 	<input>\n		<net-file value="'+map_name+'.net.xml"/>\n 		<route-files value="'+map_name+'.rou.xml"/>\n\t</input>\n\n\t<time>\n 		<begin value="0"/>\n 		<end value="1000000"/>\n 	</time>\n\n 	<gui_only>\n 		<start value="true"/>\n 	</gui_only>\n\n <tripinfo-output value="'+dirActual+'results/'+algoritmo+'/'+map_name+'.out.xml"/>\n\n <device.emissions.probability value="1.0"/> \n\n</configuration>'
 	#sumo_file.write(sumo_text)

def create_sumo_parameters(dirActual,algoritmo, map_name, request_interval_value, reroute_interval_value):
	print (dirActual+map_name+"-"+str(request_interval_value)+'-'+str(reroute_interval_value)+'-'+algoritmo+".sumo.cfg")
	#sumo_file = open(dirActual+map_name+"-"+str(request_interval_value)+'-'+str(reroute_interval_value)+'-'+algoritmo+".sumo.cfg", 'w')
	sumo_text = '<?xml version="1.0" encoding="ISO-8859-1"?>\n\n<configuration>\n\n 	<input>\n		<net-file value="'+map_name+'.net.xml"/>\n 		<route-files value="'+map_name+'.rou.xml"/>\n\t</input>\n\n\t<time>\n 		<begin value="0"/>\n 		<end value="1000000"/>\n 	</time>\n\n 	<gui_only>\n 		<start value="true"/>\n 	</gui_only>\n\n <tripinfo-output value="'+dirActual+'results/'+algoritmo+'/'+map_name+'-'+str(request_interval_value)+'-'+str(reroute_interval_value)+'.out.xml"/>\n\n <device.emissions.probability value="1.0"/> \n\n</configuration>'
 	#sumo_file.write(sumo_text)

def create_launchd_parameters(dirActual, algoritmo, map_name, request_interval_value, reroute_interval_value):
	print (dirActual+map_name+"-"+str(request_interval_value)+'-'+str(reroute_interval_value)+'-'+algoritmo+".launchd.xml")

	#launchd_file = open(dirActual+map_name+"-"+str(request_interval_value)+'-'+str(reroute_interval_value)+'-'+algoritmo+".launchd.xml", 'w')
	launchd_text = '<?xml version="1.0"?>\n\t<launch>\n\t\t<copy file="'+map_name+'.net.xml"/>\n\t\t<copy file="'+map_name+'.rou.xml"/>\n\t\t<copy file="'+map_name+"-"+str(request_interval_value)+'-'+str(reroute_interval_value)+'-'+algoritmo+'.sumo.cfg" type="config"/>\n\t\t<basedir path="'+dirActual+'"/>\n\t</launch>'
 	#launchd_file.write(launchd_text)

def net_cp(dir,net_file_name):
	shutil.copy2(net_file_name,dir)

def format_file(out):
	for line in fileinput.input(out, inplace=True):
		if (line.count("http://") > 0):
			print  ('\t<routes>')
			continue

		if(line.count("police") == 0 and line.count("color") == 0 ):
			if(line.count("id=") > 0):
				line = line.split('id="')
				line = line[1].split('"')
				print  ('\t<vehicle id="'+line[0]+'" depart="0.00">')
			else:
				print (line.rstrip('\n')) 
		else:
				print (line.rstrip('\n')) 

def create_folder(dir):
	if not os.path.exists(dir): 
		os.makedirs(dir,0o755)

def create_xml_roads_blockeds(roads,num_roads,start,duration,fileName):
	texto = []
	for i in xrange(0,int(num_roads)):
		string = "\t<edge id=\"" + str(roads[i]) + "\" start=\"" + str(start) + "\" duration=\"" +str(duration) + "\"/>\n"
		texto.append(string)

	fileOut = open(fileName, "w")
	fileOut.write("<xml>\n")
	fileOut.writelines(texto)
	fileOut.write("</xml>\n")
	fileOut.close()

def centro(str):
	x = 0.0
	y = 0.0

	points = str.split(' ')
	
	for element in points:
		coordenadas = element.split(',')
		x = x + float(coordenadas[0])
		y = y + float(coordenadas[1])

	x = x / len(points)
	y = y / len(points)
	
	centro = []
	centro.append(x)
	centro.append(y)
	return centro

def distanciaValida(id, selectedRoads, map, roadDict, minDistance):
	if len(selectedRoads) == 0:
		return True
	for element in selectedRoads:
		x1 = map[id]["point"][0]
		y1 = map[id]["point"][1]
		x2 = map[element]["point"][0]
		y2 = map[element]["point"][1]
		if  math.sqrt(math.pow(x1-x2,2) + math.pow(y1-y2,2)) < minDistance:
			return False
	return True

def getNetDict(netFileName):
	netFile = open(netFileName, "r")
	soupNet = BeautifulSoup(netFile.read(), "lxml")
	
	map = {}
	for edge_tag in soupNet.findAll("edge"):
		id = edge_tag["id"]
		map[id] = {}
		map[id]["lanes"] = str(edge_tag).count("lane") / 2 
		for lane_tag in edge_tag.findAll("lane"):
			map[id]["speed"] = lane_tag["speed"]
			map[id]["length"] = lane_tag["length"]
			map[id]["point"] = centro(lane_tag["shape"])
			break
	return map

def getRoadDict(rou_file_name):
	routeFile = open(rou_file_name, "r").read()
	soupRou = BeautifulSoup(routeFile, "lxml")

	roadDict = {}
	for route_tag in soupRou.findAll("route"):
		route_string = route_tag["edges"]
		id = route_string.split(' ')
		for id_atual in id:
			if id_atual in roadDict:
				roadDict[id_atual] = roadDict[id_atual] + 1
			else:
				roadDict[id_atual] = 1
	return roadDict

def getSelectedRoads(num_roads, netDict, roadDict, minDistance):
	orderedRoads = getOrderedRoads(roadDict)
	selectedRoads = []
	for element in orderedRoads:
		if len(selectedRoads) <= num_roads:
			if distanciaValida(element, selectedRoads, netDict, roadDict, minDistance):
				selectedRoads.append(element)
		else:
			break

	return selectedRoads


def getOrderedRoads(roadDict):
	orderedRoads = []
	for element in sorted(roadDict, key = roadDict.get,reverse = True):
		orderedRoads.append(element)
	return orderedRoads


def getStoreFolderRoadsBlockeds(dir, map_name, seed, vehicle):
	storeFolder = dir+"input/"+map_name+"/"+"seed-"+str(seed)+"/" + str(vehicle)+"/roads_blockeds_distance/"
	create_folder(storeFolder)
	return storeFolder

def getXmlFileName(diretorio, num_roads, start, duration_atual):
	return diretorio+"roads_blockeds-"+str(num_roads)+'-'+str(start)+'-'+str(duration_atual)+".xml"


def getRouFileName(dir, map_name, current_num_seed, n_vehicle):
	storeFolder = getStoreFolder(dir, map_name, current_num_seed, n_vehicle)
	rou_file_name = storeFolder+map_name+".rou.xml"
	return rou_file_name

def printSelectedRoads(selectedRoads, roadDict, seed, vehicle):
	print ("Ruas selecionadas para seed =", seed, "número de veículos =", vehicle)
	for i in xrange(1,len(selectedRoads)):
		print (str(i)+"->", selectedRoads[i-1], roadDict[selectedRoads[i-1]])

def create_roads_blockeds(dirWork, map_name, num_vehicles, num_roads_blockeds, start, duration, num_seed, net_file_name, minDistance):
	#print "Processando net file de", map_name
	netDict = getNetDict(net_file_name)
	for vehicle in num_vehicles:
		for seed in xrange(0,num_seed):	
			
			rou_file_name = getRouFileName(dirWork, map_name, seed, vehicle)

			storeFolder = getStoreFolderRoadsBlockeds(dirWork, map_name, seed, vehicle)
			
			roadDict = getRoadDict(rou_file_name)
			
			selectedRoads = getSelectedRoads(max(num_roads_blockeds), netDict, roadDict, minDistance)

			printSelectedRoads(selectedRoads, roadDict, seed, vehicle)

			for num_roads in num_roads_blockeds:
				for start_atual in start:
					for duration_atual in duration:
						xmlFileName = getXmlFileName(storeFolder, num_roads, start_atual, duration_atual)
						create_xml_roads_blockeds(selectedRoads,num_roads,start_atual,duration_atual,xmlFileName)
				

def getStoreFolder(dir, map_name, current_num_seed, n_vehicle):
	return dir+"input/"+map_name+"/"+"seed-"+str(current_num_seed)+"/"+str(n_vehicle)+"/"

def checkOS():
	""" if platform == "linux" or platform == "linux2":
	    return "/home/gomides/Documents/developement/"
	elif platform == "darwin":
	    return "/Users/gomides/Documents/development/" """
	#return "/Users/nafiulhassan/Downloads/sumo_dev/"
	return "/Users/nafiulhassan/veins-5.1/examples/veins/"

def main():
	map_name = "washingtondc"
	num_vehicles =[20, 30, 40, 50, 60, 80, 100, 120, 200]

	request_interval = [-1,0,1]
	reroute_interval = [0.1,0.2,0.3,0.4]
	#request_interval = [30]
	#reroute_interval = [50]

	num_seed = 30

	dirWork = checkOS()
	minDistance = 1000

	algoritmos = ["transient-cache"]
	net_file_name = dirWork+"input/"+map_name+"/"+str(num_vehicles[0])+"/"+map_name+".net.xml"

	for current_num_seed in range(0, num_seed):
		for n_vehicles in num_vehicles:
			print ("Processando seed =", current_num_seed, "número de veículos =", n_vehicles)
			dirActual = getStoreFolder(dirWork, map_name, current_num_seed, n_vehicles)
			print (dirActual)
			#exit(1)
			rou_file_name = dirActual+map_name+".rou.xml"
			#create_folder(dirActual)
			#create_folder(dirActual+"results/pandora/")
			#create_folder(dirActual+"results/reactive-parameters/")
			create_folder(dirActual+"results/"+algoritmos[0])

			net_cp(dirActual,net_file_name)
			generate_trip(dirWork, current_num_seed, n_vehicles, map_name, net_file_name)
			generate_route(dirWork, current_num_seed, n_vehicles, map_name, net_file_name)
			format_file(rou_file_name)

			for request_interval_value in request_interval:
				for reroute_interval_value in reroute_interval:
					if request_interval_value + 20 != reroute_interval_value: continue
					for algoritmo in algoritmos:
						create_sumo_parameters(dirActual, algoritmo, map_name, request_interval_value, reroute_interval_value)
						create_launchd_parameters(dirActual, algoritmo, map_name, request_interval_value, reroute_interval_value)
	
	#create_roads_blockeds(dirWork, map_name, num_vehicles, num_roads_blockeds, start, duration, num_seed, net_file_name, minDistance)


if __name__ == '__main__':
	main()