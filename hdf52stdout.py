#!/usr/bin/env python

"""Displays the tree structure of the PySUSY CMSSM evidence project
hdf5 database"""

#import numpy as np
import h5py
import sys

filename = sys.argv[1]  #choose file to display via command line

print filename
f = h5py.File(filename,'r')

print list(f)

horz=unichr(0x2500)
split=' '+unichr(0x251C)+2*horz
end=' '+unichr(0x2514)+2*horz
vert=' '+unichr(0x2502)+'  '
space='    '

def islast(item,iterator):
    """Check if an item is the last one in an iterator"""
    i = None
    for i in iterator:
        pass    #go through the iterator to the end
    return item==i 

def lastitem(iterator):
    """Check if an item is the last one in an iterator"""
    i = None
    for i in iterator:
        pass    #go through the iterator to the end
    return i
    
def printcontents(item,prelines):
    """print out the contents of the hdf5 directory 'item'"""
    try:
        lname,last = lastitem(item.items())
        for name,subitem in item.items():
            (line, nextpreline) = (end, space) if subitem==last else (split, vert)
            print prelines+line+name
            printcontents(subitem,prelines+nextpreline)
    except AttributeError:  #item is not a directory; assume it is a dataset and spit out its attributes instead
        print prelines+'    '+str(item.shape)
        for name,subitem in item.attrs.items():
            print prelines+'    '+name+': '+str(subitem)
        dset = item
        print dset
        print "Contents:"
        print dset[:]  
    except TypeError: # No item in list, just ignore
        pass

printcontents(f,'')

### test extraction:
import numpy as np
group = f["/"]
pid  = group["pointID"]
rank = group["MPIrank"]
pid_isvalid  = np.array(group["pointID_isvalid"],dtype=np.bool)
rank_isvalid = np.array(group["MPIrank_isvalid"],dtype=np.bool)
m = (pid_isvalid & rank_isvalid)
for p,r in zip(pid[m],rank[m]):
  if p==2281 and r==0:
     print "point (",p,",",r,") exists!"
                                        
quit()

##  #test extraction of data from dataset
##  
##  #dset=f['prelep/CCR/mu+/txt']
##  #print dset[150000:150020,0:10]
##  #dset[:,0]
##  #Wow, that is fast for small slices, and about the same as mathematica binaries 
##  #for reading through the whole thing.
##  
##  #Quick plotting test and evidence computation
##  
##  #txtdset = f['full/log/mu+/txt']
##  txtdset = f['higgs/CCR/mu+/txt']
##  nrows = txtdset.shape[0]
##  print nrows
##  print txtdset.dtype.names
##  print txtdset.dtype
##  print 'A', txtdset[0:10,'M0']
##  index = [i for i,name in enumerate(txtdset.dtype.names) if name=='M0']
##  print index
##  print 'B', txtdset[0:10].view(dtype='<f4')[:,index] #create view of structured array so we can slice it
##  
##  print 'column extraction tests:'
##  cols=['M0','M12','A0','TanB','neg2LogL']
##  for col in cols:
##      print col, txtdset[0:10][col]
##      
##  print 'double column extraction tests:'
##  cols=[['M0','M12'],['A0','TanB'],['M0','neg2LogL']]
##  for col in cols:
##      print col, txtdset[0:10][col]   #BE CAREFUL! ORDER OF OUTPUT TUPLE MAY NOT MATCH ORDER OF COLS!!!
##      print col[0], txtdset[0:10][col][col[0]]    #Extract individual cols from result to make sure correct data is obtained
##      print col[1], txtdset[0:10][col][col[1]]
##      
##  #data = np.array(txtdset[:1000,'M0','neg2LogL']).view(dtype='<f4')
##  #print data.dtype.names
##  """
##  t0 = time.time()
##  data = txtdset['M0','neg2LogL'].view(dtype='<f4').reshape(-1,2)
##  print 't:', time.time()-t0
##  
##  fig = plt.figure()
##  ax = fig.add_subplot(111)
##  line = ax.plot(data[:,0],data[:,1],'.')
##  #ax.set_ylim(logL[100000],max(logL))
##  """
##  """
##  t0 = time.time()
##  data = l.getcols(txtdset,['M0','neg2LogL'])
##  print 't:', time.time()-t0
##  
##  fig = plt.figure()
##  ax = fig.add_subplot(111)
##  line = ax.plot(data[:,0],data[:,1],'.')
##  #ax.set_ylim(logL[100000],max(logL))
##  """
##  t0 = time.time()
##  pars=['M0','TanB','neg2LogL']
##  data = l.getcols(txtdset,pars)
##  print 't:', time.time()-t0
##  
##  #l.bin2d(data,binop='min')
##  
##  fig2 = plt.figure()
##  ax = fig2.add_subplot(111)
##  plot = l.profplot(ax,data,labels=pars[0:2])
##  plt.show()
##  
##  """
##  fig2 = plt.figure()
##  ax = fig2.add_subplot(111)
##  plot = l.chi2scatplot(ax,data,labels=pars[0:2])
##  cbar = fig2.colorbar(plot, ticks=[0, 1, 2, 3, 4, 5])
##  cbar.ax.set_yticklabels(['0','1','4','9','16',r'$\geq$25'])# vertically oriented colorbar
##  plt.show()
##  """
##  quit()
##  #----------------old stuff-----------------------------
##  #get y data
##  txtdset = f['full/log/mu+/txt']
##  evdset = f['full/log/mu+/evdat']
##  livedset = f['full/log/mu+/physlive']
##  
##  nrows,ncols = txtdset.shape
##  print list(txtdset.attrs['header'][:ncols])+['logw','mode']
##  print list(txtdset.attrs['header'][-3:])
##  
##  #get logL values of rejected points
##  H = evdset.attrs['header']
##  want = ['logL','logw']
##  cols = [i for i,var in enumerate(H) if var in want]
##  data = evdset[:,cols]
##  logL = data[:,0]
##  logwO = data[:,1] #log prior weights for rejected points, computed by multinest
##  n=20000.
##  wliveO = (1-sum(np.exp(logwO))) / n #remaining prior volume divided evenly among final live set
##  logwlive0=np.log(wliveO)
##  print '========='
##  print sum(np.exp(logwO))
##  print wliveO
##  
##  #get logL values of final live set
##  H = livedset.attrs['header']
##  want = ['logL']
##  cols = [i for i,var in enumerate(H) if var in want]
##  logLlive = evdset[:,cols]
##  
##  """
##  minlogL = min(-0.5*txtdset[:,1])
##  print 'minlogL : ', minlogL 
##  #removed points with loglikelihood less than 1E-99
##  
##  cutlogL = np.array([row[:7] for row in txtdset if row[1] >= minlogL])
##  print len(cutlogL), n, len(cutlogL)+n
##  print nrows
##  
##  print cutlogL[:5]
##  print -0.5*txtdset[:5,1]
##  
##  print cutlogL[:5,:5]
##  print evdset[:5,:5]
##  
##  """
##  
##  #recompute evidence
##  
##  w=np.array([(np.exp(-(i-1.)/n) - np.exp(-(i+1.)/n))/2. for i in range(1,len(logL)+1)])    #prior weights for rejected points
##  
##  wlive=np.exp(-(len(logL)+1)/n) / n #remaining prior volume divided evenly among final live set
##  print '--------'
##  print sum(w)
##  print (1- sum(w)) / n
##  print wlive
##  wlive = (1 - sum(w)) / n
##  logw = np.log(w)
##  logwlive=np.log(wlive)
##  
##  print len(logLlive)
##  print len(logL)
##  print len(logw) 
##  print np.exp(txtdset.attrs['logZ'])
##  print txtdset.attrs['logZ']
##  Z = sum([np.exp(logLi + logwi) for logLi,logwi in zip(logL,logw)])
##  Z += sum(np.exp(logLlive + logwlive))
##  logZ = np.log(Z)
##  HZ = sum([np.exp(logLi + logwi - logZ) * (logLi-logZ) for logLi,logwi in zip(logL,logw)])
##  dlogZ = np.sqrt(HZ/n)
##  print Z
##  print logZ,'+/-',dlogZ
##  Z2 = sum([np.exp(logLi + logwi) for logLi,logwi in zip(logL,logwO)])
##  logZ2 = np.log(Z2)
##  #Z2 += sum(np.exp((ylive + logwliveO)+
##  HZ2 = sum([np.exp(logLi + logwi - logZ2) * (logLi-logZ2) for logLi,logwi in zip(logL,logwO)])
##  dlogZ2 = np.sqrt(HZ2/n)
##  print Z2
##  print logZ2,'+/-',dlogZ2 
##  
##  #wicked! That worked out nicely.
##  
##  fig = plt.figure()
##  ax = fig.add_subplot(111)
##  line = ax.plot(range(1,len(logL)+1),logL,'.')
##  ax.set_ylim(logL[100000],max(logL))
##  plt.show()

