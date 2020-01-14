"""
in cpp program, enable option DEBUG_SHOW_FPS
it will print foreach loop (in main thread) the time for one loop (millisecond):
FPS: |threadName| <millisecond>
at the end, print
ENDFPS
"""

import matplotlib.pyplot as plt
import numpy as np
import time

fpsHeader = "FPS: "
threadNamesep = "|"
fpsEnd = "ENDFPS"
allFps = {}
firstMillis = -1
while 1:
	try:
		try:
			a = input()
		except:
			break
		if a[:len(fpsHeader)] == fpsHeader:
			name = a.split(threadNamesep)[1].strip()
			ms = int(a.split(threadNamesep)[2].strip())
			curMillis = int(round(time.time() * 1000))
			if ms < curMillis - 1000 or ms > curMillis + 1000:
				continue
			if (firstMillis == -1):
				firstMillis = ms
			count = int(a.split(threadNamesep)[3].strip())
			if count <= 0 or count > 10000:
				continue
			if name not in allFps:
				allFps[name] = []
			allFps[name].append([(ms - firstMillis) / 1000, 1000 / count])
		elif a == fpsEnd:
			break
		else:
			print(a)
	except:
		continue

color = [
	'red',
	'blue',
	'green',
	'yellow',
	'lightblue',
	'chocolate',
	'black',
]
i = 1
for name in allFps:
	arr = np.array(allFps[name])
	plt.title("FPS")
	plt.xlabel("time (sec)")
	plt.ylabel("FPS")
	plt.plot(arr[:,0], arr[:,1], label=name, color=color[i % len(color)])
	i += 1
plt.legend()
plt.show()
