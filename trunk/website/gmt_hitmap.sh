#!/bin/sh
#	$Id: gmt_hitmap.sh,v 1.8 2007-12-20 19:35:16 guru Exp $
#
# Make the GMT ftpsite hitmap PNG image and the web page that uses it.
# The files created are gmt_hitmap.png and gmt_hitmap.map
#
dpi=100
yoff=1
dia=0.2
#-------------------------------------------------------------------------------------------------
# ADD NEW MIRRORS HERE (Also fix install script etc)
# Remember: Only ONE TAB between fields, otherwise the next awk gets confused.
cat << EOF > mirrors.d
-77:0	38:52	ftp://ibis.grdl.noaa.gov/pub/gmt	CT	SILVER SPRING - USA	NOAA, Lab for Satellite Altimetry, Silver Spring, Maryland, USA	Serving East North America
-122:22	47:40	ftp://ftp.iris.washington.edu/pub/gmt	CB	SEATTLE - USA	IRIS (Incorporated Research Institutions for Seismology), Seattle, Washington, USA	Serving West North America
-46:40	-23:32	ftp://ftp.iag.usp.br/pub/gmt	CT	S\303O PAULO - BRAZIL	IAG-USP, Dept of Geophysics, S&#227;o Paulo, Brazil	Serving South America
10:44	59:55	ftp://ftp.geologi.uio.no/pub/gmt	CB	OSLO - NORWAY	IFG, Dept of Geosciences, Oslo, Norway	Serving Europe
16:22	48:12	ftp://gd.tuwien.ac.at/pub/gmt	CT	VIENNA - AUSTRIA	Vienna U of Techology, Vienna, Austria	Serving Europe
138:30	35:00	ftp://ftp.scc.u-tokai.ac.jp/pub/gmt	CB	SHIMIZU - JAPAN	Tokai U, Shimizu, Japan	Serving Asia
EOF
cat << EOF > master.d
-157:59	21:55	ftp://ftp.soest.hawaii.edu/gmt	CT	HONOLULU - USA	SOEST, Dept of Geology & Geophysics, Honolulu, Hawaii, USA	Serving the Pacific Rim
EOF
#-------------------------------------------------------------------------------------------------
if [ $# -eq 1 ]; then
	gush=0
else
	gush=1
fi
if [ $gush ]; then
	echo "gmt_hitmap.sh: Preparing the web page hitmap image"
fi
awk -F'\t' '{printf "%s\t%s\t9\t0\t1\t%s\t%s\n", $1, $2, $4, $5}' mirrors.d > mirror_sites.d
awk -F'\t' '{printf "%s\t%s\t12\t0\t1\t%s\t%s\n", $1, $2, $4, $5}' master.d > master_site.d

pscoast -Rd -JN180/6i -Slightblue -Gbrown -Dc -A2000 -B0g30/0g15 -K -P -Y${yoff}i --PAPER_MEDIA=letter+ --CHAR_ENCODING=ISOLatin1+ > gmt_hitmap.ps
# Draw spokes from Hawaii to each site
i=1
while read lon lat rest; do
	cut -f1,2 master.d > t
	echo "$lon $lat" >> t
	psxy -R -J -O -K -W2p,darkgreen t -A >> gmt_hitmap.ps
done < mirror_sites.d
# Place yellow and red circles
psxy -R -J -O -K -Sc${dia}i -Gyellow -Wthin mirror_sites.d >> gmt_hitmap.ps
psxy -R -J -O -K -Sc${dia}i -Gred -Wthin master_site.d >> gmt_hitmap.ps
# Add site labels
pstext -R -J -O -K mirror_sites.d -W255O -N -Dj0.15i/0.175i >> gmt_hitmap.ps
pstext -R -J -O -K master_site.d -W0O -Gwhite -N -Dj0.15i/0.2i >> gmt_hitmap.ps
# Draw the legend
pslegend -R0/5/0/1 -Jx1i -O -K -D3/0/2.1i/0.6i/CT -Gtan -Y-0.1i -F2p -L1.25 --ANNOT_FONT_PRIMARY=Helvetica-Bold << EOF >> gmt_hitmap.ps
S 0.2i c 0.2i red 0.25p 0.45i GMT Master Site
S 0.2i c 0.2i yellow 0.25p 0.45i GMT Mirror Site
EOF
psxy -R -J -O /dev/null >> gmt_hitmap.ps
ps2raster -A -E$dpi -Tg gmt_hitmap.ps
if [ $gush ]; then
	xv gmt_hitmap.png &
fi
file gmt_hitmap.png > $$
IW=`awk '{print $5}' $$`
IH=`awk '{print $6}' $$`
if [ $gush ]; then
	echo "gmt_hitmap.sh: Preparing gmt_mirrors.html"
fi
H=`echo 180 90 | mapproject -JN180/6i -Rd | cut -f2`
width=`gmtmath -Q 6 $dpi MUL =`
height=`gmtmath -Q $H $dpi MUL =`
rad=`gmtmath -Q $dia $dpi MUL 2 DIV =`
cat << EOF > gmt_mirrors.html
<HTML>
<!--    gmt_mirrors.html [Generated by gmt_mirrors.sh]      -->
<HEAD>
<TITLE>GMT - The Generic Mapping Tools</TITLE>
</HEAD>
<BODY bgcolor="#ffffff">
<CENTER><H2>GMT Master and Mirror FTP Sites</H2></CENTER>
You only need to visit one of these sites if you cannot (or will not) do the automatic
install described under the <A HREF="gmt_download.html">Download</A> section. This is most likely true
if you just want to obtain Windows executables. To connect to your preferred ftp server, click on the
location that is physically nearest you:<P>
<P><CENTER><IMG SRC="gmt_hitmap.png" WIDTH="$IW" HEIGHT="$IH" ALIGN=bottom NATURALSIZEFLAG="3" USEMAP="#GMTmap"> </A></CENTER>
<MAP name="#GMTmap">
EOF
cat mirrors.d master.d | mapproject -JN180/$width -Rd | awk -F'\t' '{printf "\t<AREA HREF=\"%s\" SHAPE=\"circle\" COORDS=\"%d,%d,%d\">\n", $3, int($1+0.5), int('$height'-$2+0.5), int('$rad'+0.5)}' >> gmt_mirrors.html
cat << EOF >> gmt_mirrors.html
</MAP>
<P>
Alternatively, click on the corresponding text link below:
<OL>
EOF
cat master.d mirrors.d | awk -F'\t' '{printf "<LI><A HREF=\"%s\">%s</A>.  %s\n", $3, $6, $7}' >> gmt_mirrors.html
cat << EOF >> gmt_mirrors.html
</OL>
</BODY>
</HTML>
EOF
rm -f mirrors.d master.d mirror_sites.d master_site.d gmt_hitmap.ps $$
