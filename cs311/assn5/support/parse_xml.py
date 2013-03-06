from xml.dom import minidom
xmldoc = minidom.parse('example.xml')
itemlist = xmldoc.getElementsByTagName('data') 
for s in itemlist :
    print s.attributes['value'].value