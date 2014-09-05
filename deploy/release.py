import subprocess,os,sys
"""Used by Release hudson job and does the following:

  1) Find the release tags. They will have the names 'flavor'_release-m-n-o where:
        'flavor' is one of alpha, beta or stable
        m = major version number
        n = minor version number
        o = release number
  2) For each 'flavor':
    2a) find the tag with the highest version and release.
    2b) See if any changes were made on the cvs branch associated with the flavor since
        the modules were tagged with this latest release.
    2c) If changes:
        2c1) Checkout a copy of the branch
        2c2) Make a tar file in ${MDSPLUS_DIST}/SOURCES with name=mdsplus[-flavor]-m-n-o(+1).tgz
        2c3) Add a new tag to the branch with release incremented

"""

def flushPrint(text):
  print(text)
  sys.stdout.flush()

def makeAllSourceTars():
  """Create missing source tarballs"""
  p=subprocess.Popen("cvs -Q rlog -h -S mdsplus/configure.in | grep _release",stdout=subprocess.PIPE,shell=True)
  tags=p.stdout.readlines()
  p.wait()
  for tag in tags:
    tag=tag.split(':')[0].lstrip()
    v=tag.split('-')[1:]
    major=int(v[0])
    minor=int(v[1])
    release=int(v[2])
    flavor=tag.split('-')[0].split('_')[0]
    if flavor == "stable":
      rflavor=""
    else:
      rflavor="-%s" % flavor
    src="mdsplus%s-%d.%d-%d" % (rflavor,major,minor,release)
    tarball='${MDSPLUS_DIST}/SOURCES/'+src+'.tgz'
    try:
      os.stat(tarball)
    except:
      print "%s not found, creating" % tarball
      status=subprocess.Popen("""
rm -Rf %(src)s
cvs -Q -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co -d %(src)s -r %(tag)s mdsplus
tar zhcf %(tarball)s --exclude CVS %(src)s
rm -Rf %(src)s
""" % {'tag':tag,'tarball':tarball,'src':src},shell=True,cwd="/tmp").wait()

def getLatestRelease(flavor):
  """Get latest releases for all flavors"""
#  Get release tags
  p=subprocess.Popen("cvs -Q rlog -h -S mdsplus/configure.in  | grep %(flavor)s_release" % {'flavor':flavor},
                     stdout=subprocess.PIPE,shell=True)
  tags=p.stdout.readlines()
  p.wait()

# For each flavor
  info=dict()
  info['flavor']=flavor
  if flavor == "alpha":
    info['branch']="HEAD"
  else:
    info['branch']=flavor
  if flavor == "stable":
    info['rflavor']=""
  else:
    info['rflavor']="-"+flavor
#   Find "latest" release tag
  latest=0
  for tag in tags:
    if flavor in tag:
      tag=tag.split(':')[0].lstrip()
      v=tag.split('-')[1:]
      vers=(int(v[0])<<24)+(int(v[1])<<16)+(int(v[2]))
      if vers > latest:
        info['tag']=tag
        info['major']=int(v[0])
        info['minor']=int(v[1])
        info['release']=int(v[2])
        latest=vers
  return info

def processChanges(flavor):
  """Check if any changes were made since latest releases and create new release"""
#    See if anything has changed
  info=getLatestRelease(flavor)
  p=subprocess.Popen("cvs -Q rdiff -s -r %s -r %s mdsplus" % (info['tag'],info['branch']),
                     stdout=subprocess.PIPE,shell=True)
  changes=p.stdout.readlines()
  info['numchanges']=len(changes)
  p.wait()
  flushPrint(str(info()))
  if info['numchanges'] > 0:
    #    If changes
    flushPrint("There were %(numchanges)d changes to the %(branch)s branch since release %(tag)s" % info)
    for change in changes:
      flushPrint("     %s" % change)
    info['release']=info['release']+1
    info['tag'] = "%(branch)s_release-%(major)d-%(minor)d-%(release)d" % info
  #      Checkout the source and make a source tarball and if successful tag the new release
  flushPrint(str(info()))
  info['src']="mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d" % info
  status=subprocess.Popen("""
set -e
if [ ! -r ${MDSPLUS_DIST}/SOURCES/%(src)s.tgz ]
then
  echo ${MDSPLUS_DIST}/SOURCES/%(src)s.tgz not found. Creating source tarball.
  rm -Rf mdsplus-*
  cvs -Q -d :pserver:MDSguest:MDSguest@www.mdsplus.org:/mdsplus/repos co -d %(src)s -r %(branch)s mdsplus
  cd %(src)s
  if [ %(numchanges)d -gt 0 ]
  then
    cvs -Q tag %(tag)s
  fi
  echo "%(tag)s" > header.tmp
  echo "" >> header.tmp
  echo "" >> header.tmp
  touch ChangeLog
  devscripts/cvs2cl.pl --prune --accum --header header.tmp --separate-header -b 2> /dev/null
  rm header.tmp
  cd ..
  tar zhcf ${MDSPLUS_DIST}/SOURCES/%(src)s.tgz --exclude CVS %(src)s
  cd /tmp
  rm -Rf mdsplus-*
else
  echo ${MDSPLUS_DIST}/SOURCES/%(src)s.tgz exists
fi
""" % info,shell=True,cwd="/tmp").wait()
  if status != 0:
    raise Exception("Error handling new release")

if __name__ == "__main__":
  if len(sys.argv) == 1 or sys.argv[1]=='release':
    for flavor in ('stable','beta','alpha'):
      processChanges(flavor)
  elif sys.argv[1]=='deploy':
    if len(sys.argv) == 3:
      flavors=sys.argv[2].split(',')
    elif 'BUILD_FLAVOR' in os.environ and os.environ['BUILD_FLAVOR'] in ('alpha','beta','stable'):
      flavors=[os.environ['BUILD_FLAVOR'],]
    else:
      flavors=('alpha','beta','stable')
    errors=""
    for flavor in flavors:
      processChanges(flavor)
      info = getLatestRelease(flavor)
      info['executable']=sys.executable
      if subprocess.Popen("""
set -e
rm -Rf mdsplus%(rflavor)s-?.* %(flavor)s
tar zxf ${MDSPLUS_DIST}/SOURCES/mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d.tgz mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d/deploy
cd mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d/deploy
%(executable)s  deploy.py %(flavor)s %(major)s %(minor)d %(release)d
""" % info,shell=True).wait() != 0:
        error="Deploy failed for mdsplus%(rflavor)s-%(major)d.%(minor)d-%(release)d" % info
        flushPrint("x"*100+"\n\n%s\n\n" % error + "x"*100)
        errors=errors+error+"\n"
    if len(errors) > 0:
      sys.exit(1)
  elif sys.argv[1]=='sources':
    makeAllSourceTars()
