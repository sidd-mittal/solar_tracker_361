import math
import numpy as np
from matplotlib import pyplot as plt
import matplotlib.cm as cm
import pandas as pd

#https://www.kaggle.com/code/benwatson/sun-position-function/script
def sunPosition(year, month, day, hour=12, m=0, s=0,lat=43.5, long=-80.5):

    twopi = 2 * math.pi
    deg2rad = math.pi / 180

    # Get day of the year, e.g. Feb 1 = 32, Mar 1 = 61 on leap years
    len_month = [0,31,28,31,30,31,30,31,31,30,31,30]
    day = day + np.cumsum(len_month)[month-1]
    leapdays = (year % 4 == 0 and (year % 400 == 0 | year % 100 != 0) \
                and day >= 60 and not (month==2 and day==60))
    day += int(leapdays == True)

    # Get Julian date - 2400000
    hour = hour + m / 60 + s / 3600 # hour plus fraction
    delta = year - 1949
    leap = math.floor(delta / 4) # former leapyears
    jd = 32916.5 + delta * 365 + leap + day + hour / 24

    # The input to the Atronomer's almanach is the difference between
    # the Julian date and JD 2451545.0 (noon, 1 January 2000)
    time = jd - 51545.

    # Ecliptic coordinates

    # Mean longitude
    mnlong = 280.460 + .9856474 * time
    mnlong = mnlong % 360
    mnlong += int(mnlong < 0)*360

    # Mean anomaly
    mnanom = 357.528 + .9856003 * time
    mnanom = mnanom % 360
    mnanom += int(mnanom < 0)*360
    mnanom = mnanom * deg2rad

    # Ecliptic longitude and obliquity of ecliptic
    eclong = mnlong + 1.915 * math.sin(mnanom) + 0.020 * math.sin(2 * mnanom)
    eclong = eclong % 360
    eclong += int(eclong < 0)*360
    oblqec = 23.439 - 0.0000004 * time
    eclong = eclong * deg2rad
    oblqec = oblqec * deg2rad

    # Celestial coordinates
    # Right ascension and declination
    num = math.cos(oblqec) * math.sin(eclong)
    den = math.cos(eclong)
    ra = math.atan(num / den)
    ra += int(den < 0)*math.pi
    ra += int(den >= 0 and num < 0)*twopi
    dec = math.asin(math.sin(oblqec) * math.sin(eclong))

    # Local coordinates
    # Greenwich mean sidereal time
    gmst = 6.697375 + .0657098242 * time + hour
    gmst = gmst % 24
    gmst += int(gmst < 0)*24

    # Local mean sidereal time
    lmst = (gmst + long / 15.)
    lmst = lmst % 24.
    lmst += int(lmst < 0)*24.
    lmst = lmst * 15. * deg2rad

    # Hour angle
    ha = lmst - ra
    ha += int(ha < -math.pi)*twopi
    ha -= int(ha > math.pi)*twopi

    # Latitude to radians
    lat = lat * deg2rad

    # Azimuth and elevation
    el = math.asin(math.sin(dec) * math.sin(lat) + math.cos(dec) * math.cos(lat) * math.cos(ha))
    az = math.asin(-math.cos(dec) * math.sin(ha) / math.cos(el))

    # For logic and names, see Spencer, J.W. 1989. Solar Energy. 42(4):353
    cosAzPos = (0 <= math.sin(dec) - math.sin(el) * math.sin(lat))
    sinAzNeg = (math.sin(az) < 0)
    az += int(cosAzPos and sinAzNeg)*twopi
    if (not cosAzPos):
        az = math.pi - az 

    el = el / deg2rad
    az = az / deg2rad
    lat = lat / deg2rad

    return az, el