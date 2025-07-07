import os

def create_sumo_parameters(dirActual, map_name, seed_no, veh_no):
    sumo_file = open(dirActual + map_name+'.sumo.cfg', 'w')
    sumo_text = """<!-- washingtondc.sumo.cfg -->\n
    <configuration>
    \t<input>
    \t\t<net-file value="{dir}{map}.net.xml"/>
    \t\t<route-files value="{dir}{map}.rou.xml"/>
    \t\t<additional-files value="{dir}{map}.poly.xml"/>
    \t</input>
    \t<time>
    \t\t<begin value="0"/>
    \t\t<end value="10000"/>
    \t\t<step-length value="0.1"/>
    \t</time>
    \t<processing>
    \t\t<time-to-teleport value="-1"/>
    \t</processing>
    \t<gui_only>
    \t\t<start value="true"/>
    \t</gui_only>
    </configuration>""".format(dir=dirActual, map=map_name)
    #print(sumo_text)
    sumo_file.write(sumo_text)
    sumo_file.close()

def create_launchd_parameters(dirActual, map_name, seed_no, veh_no):
    launchd_file = open(dirActual + map_name+'.launchd.xml', 'w')
    launchd_text = """<?xml version="1.0"?>
    <!-- {map}.launchd.xml -->\n
    <launch>
    \t<basedir path="{dir}" />
    \t<copy file="{map}.net.xml"/>
    \t<copy file="{map}.rou.xml"/>
    \t<copy file="{map}.poly.xml"/>
    \t<copy file="{map}.sumo.cfg" type="config"/>
    </launch>""".format(dir=dirActual, map=map_name)
    launchd_file.write(launchd_text)
    launchd_file.close()
    
def main():
    map_name = "washingtondc"
    num_seed = 30
    num_vehicles =[20, 30, 40, 50, 60, 80, 100, 120, 200]
    dir = "/Users/nafiulhassan/veins-5.1/examples/veins/"
    
    for i in range(0, num_seed):
        for j in num_vehicles:
            os.system('cp -R washingtondc.* seed-'+str(i)+'/'+str(j)+'/')
            dirActual = dir+"input/"+map_name+"/"+"seed-"+str(i)+"/"+str(j)+"/"
            create_sumo_parameters(dirActual, map_name, i, j)
            create_launchd_parameters(dirActual, map_name, i, j)
if __name__ == '__main__':
	main()