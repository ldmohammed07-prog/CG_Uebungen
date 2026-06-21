import os
import math
os.makedirs("frames", exist_ok=True)
for angle in range(0, 360):
    dx = math.cos(angle*math.pi/180)
    dy = math.sin(angle*math.pi/180)
    pattern = "./renderpipeline -s bunny -w 720 -h 480 -p {},{},50 -u 0,0,1 -d {},{},0 -o frames/rot-{:03d}.png"
    cmd = pattern.format(150*dx,150*dy,-dx,-dy,angle)
    os.system(cmd)
    print("\r{}/360".format(angle+1), end='')

print("")
print("ffmpeg...")
os.system("cat frames/rot-*png | ffmpeg -f image2pipe -r 30 -i - -vcodec libx264 rot.mkv -y")