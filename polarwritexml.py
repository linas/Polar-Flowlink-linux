# coding: utf-8
import urllib2
import re
from lxml import etree

username = 'username'
password = 'password'
send = 1
dictionary = eval(open("polardata").read())
#usersettings = {'maxhr':196,'resthr':58,'weight':70,'height':179,'vo2max':44,'sex':'male','activity':'middle','birthdate':'1977-08-13'}
usersettings = dictionary['usersettings']

xmlTemplateUserSettings = """<object name="userSettings"><prop name="maxhr">%(maxhr)s</prop><prop name="resthr">%(resthr)s</prop><prop name="vo2max">%(vo2max)s</prop><prop name="weight">%(weight)s</prop><prop name="height">%(height)s</prop><prop name="sex">%(sex)s</prop><prop name="activity">%(activity)s</prop><prop name="birthdate">%(birthdate)s</prop></object>"""
xmlTemplate = """<?xml version="1.0" encoding="UTF-8"?><wristunit-data><prop name="Username">%(username)s</prop><prop name="clientVersion">2.4.11.453</prop><prop name="model">12</prop><prop name="submodel">166</prop><prop name="version">2</prop>
  %(usersettings)s
  <collection name="Exercises">%(item)s</collection><collection name="tests"/>
</wristunit-data>"""
xmlTemplateExercice = """<item type="Exercise">
      <prop name="origin">1</prop><prop name="time">%(time)s</prop><prop name="monitor">12</prop><prop name="submodel"/><prop name="units">0</prop>
      <prop name="length">%(duration)s</prop><prop name="recordingrate">5</prop><prop name="note">%(note)s</prop><prop name="exercisename"/><prop name="save">True</prop>
        <object name="mode"><prop name="speed">True</prop><prop name="cadence">True</prop><prop name="altitude">True</prop><prop name="power">False</prop><prop name="powerlrb">False</prop><prop name="powerpi">False</prop><prop name="airpr">False</prop></object>
        <object name="userSettings"><prop name="resthr">%(userresthr)s</prop><prop name="maxhr">%(usermaxhr)s</prop></object>
        <object name="summary"><prop name="ascent"></prop><prop name="descent"></prop><prop name="energy">%(exercicecal)s</prop><prop name="hrmin">%(exercicehrmin)s</prop><prop name="hravg">%(exercicehravg)s</prop><prop name="hrmax">%(exercicehrmax)s</prop><prop name="altitudemin"></prop><prop name="altitudeavg"></prop><prop name="altitudemax"></prop><prop name="cadenceavg"></prop><prop name="cadencemax"></prop><prop name="distance"></prop><prop name="speedavg"></prop><prop name="speedmax"></prop></object>
      <collection name="limits">
        %(zones)s
      </collection>
      <collection name="laps">
        %(laps)s
      </collection>
    </item>"""
xmlTemplateLap = """<item type="Lap"><prop name="origin">1</prop><prop name="id">%(id)s</prop><prop name="laptime">%(laptime)s</prop><prop name="hr">%(hr)s</prop><prop name="hravg">%(hravg)s</prop><prop name="hrmax">%(hrmax)s</prop><prop name="speed">0</prop><prop name="speedavg">0</prop><prop name="lapdistance"></prop><prop name="altitude">0</prop><prop name="ascent">0</prop><prop name="temperature">0</prop></item>"""
xmlTemplateSportzone = """<item type="Sportzone"><prop name="origin">1</prop><prop name="id">%(id)s</prop><prop name="inzone">%(inzone)s</prop><prop name="upper">%(upper)s</prop><prop name="lower">%(lower)s</prop></item>"""


def fillDict( origdict,key,data,template):
  datas = []
  for item in reversed(data):
    datas.append(template%item)
  origdict[key]= ' '.join(datas)
  return origdict;


def callPolarPersonalTrainer(page,user,pwd,content):
    password_mgr = urllib2.HTTPPasswordMgrWithDefaultRealm()
    password_mgr.add_password(None, 'https://www.polarpersonaltrainer.com/weblink2/', user, pwd)
    handler = urllib2.HTTPBasicAuthHandler(password_mgr)
    opener = urllib2.build_opener(handler)
    urllib2.install_opener(opener)
    req = urllib2.Request(url='https://www.polarpersonaltrainer.com/weblink2/'+page, data=content ,headers = {'Content-Type' : 'text/xml','Content-Disposition' : 'form-data; name="parameters"; filename="parameters.xml"'})
    result = urllib2.urlopen(req)
    resultread =  result.read()
    print resultread
    return resultread

exercicesInWatch = {}
exercicestocheck= []

excers = dictionary['exercices']
for exercice in excers:
  index = exercice['id']
  del exercice['id']
  exercice['userresthr']=usersettings['resthr']
  exercice['usermaxhr']=usersettings['maxhr']
  lapsdata = exercice['laps']
  del exercice['laps']
  exercice = fillDict(exercice,'laps',lapsdata,xmlTemplateLap)
  zonesdata = exercice['zones']
  for zone in zonesdata:
    m=re.search(r"P(\d+)H(\d+)M(\d+)S",zone['zonetime'])
    zone['inzone']=int(m.group(1))*3600+int(m.group(2))*60+int(m.group(3))
    zone['lower']=int(usersettings['maxhr']*zone['min']/100)
    zone['upper']=int(usersettings['maxhr']*zone['max']/100)
  del exercice['zones']
  exercice = fillDict(exercice,'zones',zonesdata,xmlTemplateSportzone)
  # now we add it to the list of exercices
  exercicesInWatch[str(index)]=exercice
  exercicestocheck.append('<item type="Exercise"><prop name="time">'+exercice['time']+'</prop><prop name="origin">1</prop></item>')

#when all exercices are read
xmlForChecking='<wristunit-data><prop name="Username">'+username+'</prop><collection name="Exercises">'+' '.join(exercicestocheck)+'</collection></wristunit-data>'
print "We now have to check which exercices are already in polarpersonaltrainer"
print xmlForChecking
if send :
  print "we parse response and delete exercices already transfered"
  root = etree.fromstring(callPolarPersonalTrainer('check.jsp',username, password, xmlForChecking))
  for child in root[1].iterchildren():
    del exercicesInWatch[child.get('index')]
  print "we transfer missing exercices"
  if len(exercicesInWatch)>0:
    data = {'username':username,'usersettings':xmlTemplateUserSettings%usersettings}
    print xmlTemplate%fillDict(data,'item',exercicesInWatch.values(),xmlTemplateExercice)
    callPolarPersonalTrainer('store.jsp',username, password, xmlTemplate%fillDict(data,'item',exercicesInWatch.values(),xmlTemplateExercice))
  else:
    print "No new exercice to store !"
