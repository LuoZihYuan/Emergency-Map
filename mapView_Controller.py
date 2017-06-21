""""""
# XXX: Subclass Frame from Tkinter
# Third Party Library
import numpy as np
import matplotlib
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from mpl_toolkits.basemap import Basemap
# Self Written Library
from LoRaGPS_Helper import LoRaGPSListener

# system settings
matplotlib.rcParams['toolbar'] = 'None'
# self defined type
SIGTYPE = [('sid', str, 1),
           ('gps', float, 2)]
PNTTYPE = [('position', float, 2),
           ('size', float, 1),
           ('color', float, 4),
           ('duration', int, 1)]

signals = np.zeros(0, dtype=SIGTYPE)
points = np.zeros(0, dtype=PNTTYPE)

fig = plt.figure()
fig.canvas.set_window_title('Emergency Map')

EARTH = Basemap(projection='mill', resolution='f',
                urcrnrlat=25.437273, urcrnrlon=122.204541, llcrnrlat=24.639907, llcrnrlon=120.683063)
EARTH.drawcoastlines(linewidth=0.25)
EARTH.fillcontinents(color='0.95')

ax = plt.subplot()
scat = ax.scatter(points['position'][:, 0], points['position'][:, 1], points['size'], lw=0.5,
                  edgecolors=points['color'], facecolors=(points['color']*(1, 1, 1, 0.25)), zorder=10)
ANIMATION_INTERVAL = 10
def update(Frame):
    """  """
    global signals, points
    points['size'] *= 1.02
    points['duration'] -= ANIMATION_INTERVAL
    expired = np.where(points['duration'] <= 0)[0]
    signals = np.delete(signals, expired, 0)
    points = np.delete(points, expired, 0)
    scat.set_offsets(points['position'])
    scat.set_sizes(points['size'])
    scat.set_edgecolors(points['color'])
    scat.set_facecolors(points['color']*(1, 1, 1, 0.25))
animation = FuncAnimation(fig, update, interval=ANIMATION_INTERVAL)

# get signals
def didReceiveGPS(sid, gpsLocation):
    global signals, points
    new_signal = np.array([(sid, gpsLocation)], dtype=SIGTYPE)
    lon, lat = gpsLocation
    new_point = np.array([(EARTH(lon, lat), 5, (0, 0, 1, 1), 1000)], dtype=PNTTYPE)
    signals = np.concatenate((signals, new_signal))
    points = np.concatenate((points,new_point))
listener = LoRaGPSListener(didReceiveGPS)
listener.start()

plt.show()
listener.stop()
