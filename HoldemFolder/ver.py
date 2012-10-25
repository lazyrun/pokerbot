#! /usr/bin/python
# -*- coding: utf-8 -*-

import datetime

def IncVer():
   try:
      in_f=open('ver.h','r');
      lines = in_f.readlines()
      in_f.close();
   except IOError:
      lines = []
   
   if len(lines)==0:
      lines.append('#ifndef _VER_H_\n')
      lines.append('#define _VER_H_\n')
      lines.append('#define VERSION "1.1.1.1"\n')
      lines.append('#define RELEASE_DATE "2011-01-01"\n')      
      lines.append('#endif /*_VER_H_*/\n')
   else:
      for l in lines:
         if l.find('VERSION') >= 0:
            ver_no = l.split()
            if len(ver_no)==3:
               ver_no[2] = ver_no[2].replace('"','')
                  
               ver_parts = ver_no[2].split('.')
               if len(ver_parts)==4:
                  ver_parts[3] = int(ver_parts[3])+1
                  ver_no[2] = '"'+str(ver_parts[0])+'.'+str(ver_parts[1])+'.'+str(ver_parts[2])+'.'+str(ver_parts[3])+'"'
                  new_l = ver_no[0]+' '+ver_no[1]+' '+ver_no[2]+'\n'
                  lines[lines.index(l)] = new_l
         if l.find('RELEASE_DATE') >= 0:
            today = datetime.date.today()
            new_l = '#define RELEASE_DATE "' + str(today) + '"\n'
            lines[lines.index(l)] = new_l
         
   out_f=open('ver.h','w')
   for l in lines:
      out_f.write(str(l))
   out_f.close();
   
IncVer()
