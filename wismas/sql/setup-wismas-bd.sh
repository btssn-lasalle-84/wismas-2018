#!/bin/sh
# This script was generated using Makeself 2.2.0

ORIG_UMASK=`umask`
if test "n" = n; then
    umask 077
fi

CRCsum="1503710938"
MD5="960c050d9db72bf025de2374fc0dde34"
TMPROOT=${TMPDIR:=/tmp}
USER_PWD="$PWD"; export USER_PWD

label="Script d'installation by tv (bts sn-ir)"
script="./wismas-bd.sh"
scriptargs=""
licensetxt=""
helpheader=''
targetdir="wismas-bd"
filesizes="4796"
keep="n"
nooverwrite="n"
quiet="n"

print_cmd_arg=""
if type printf > /dev/null; then
    print_cmd="printf"
elif test -x /usr/ucb/echo; then
    print_cmd="/usr/ucb/echo"
else
    print_cmd="echo"
fi

unset CDPATH

MS_Printf()
{
    $print_cmd $print_cmd_arg "$1"
}

MS_PrintLicense()
{
  if test x"$licensetxt" != x; then
    echo "$licensetxt"
    while true
    do
      MS_Printf "Please type y to accept, n otherwise: "
      read yn
      if test x"$yn" = xn; then
        keep=n
	eval $finish; exit 1
        break;
      elif test x"$yn" = xy; then
        break;
      fi
    done
  fi
}

MS_diskspace()
{
	(
	if test -d /usr/xpg4/bin; then
		PATH=/usr/xpg4/bin:$PATH
	fi
	df -kP "$1" | tail -1 | awk '{ if ($4 ~ /%/) {print $3} else {print $4} }'
	)
}

MS_dd()
{
    blocks=`expr $3 / 1024`
    bytes=`expr $3 % 1024`
    dd if="$1" ibs=$2 skip=1 obs=1024 conv=sync 2> /dev/null | \
    { test $blocks -gt 0 && dd ibs=1024 obs=1024 count=$blocks ; \
      test $bytes  -gt 0 && dd ibs=1 obs=1024 count=$bytes ; } 2> /dev/null
}

MS_dd_Progress()
{
    if test x"$noprogress" = xy; then
        MS_dd $@
        return $?
    fi
    file="$1"
    offset=$2
    length=$3
    pos=0
    bsize=4194304
    while test $bsize -gt $length; do
        bsize=`expr $bsize / 4`
    done
    blocks=`expr $length / $bsize`
    bytes=`expr $length % $bsize`
    (
        dd ibs=$offset skip=1 2>/dev/null
        pos=`expr $pos \+ $bsize`
        MS_Printf "     0%% " 1>&2
        if test $blocks -gt 0; then
            while test $pos -le $length; do
                dd bs=$bsize count=1 2>/dev/null
                pcent=`expr $length / 100`
                pcent=`expr $pos / $pcent`
                if test $pcent -lt 100; then
                    MS_Printf "\b\b\b\b\b\b\b" 1>&2
                    if test $pcent -lt 10; then
                        MS_Printf "    $pcent%% " 1>&2
                    else
                        MS_Printf "   $pcent%% " 1>&2
                    fi
                fi
                pos=`expr $pos \+ $bsize`
            done
        fi
        if test $bytes -gt 0; then
            dd bs=$bytes count=1 2>/dev/null
        fi
        MS_Printf "\b\b\b\b\b\b\b" 1>&2
        MS_Printf " 100%%  " 1>&2
    ) < "$file"
}

MS_Help()
{
    cat << EOH >&2
${helpheader}Makeself version 2.2.0
 1) Getting help or info about $0 :
  $0 --help   Print this message
  $0 --info   Print embedded info : title, default target directory, embedded script ...
  $0 --lsm    Print embedded lsm entry (or no LSM)
  $0 --list   Print the list of files in the archive
  $0 --check  Checks integrity of the archive

 2) Running $0 :
  $0 [options] [--] [additional arguments to embedded script]
  with following options (in that order)
  --confirm             Ask before running embedded script
  --quiet		Do not print anything except error messages
  --noexec              Do not run embedded script
  --keep                Do not erase target directory after running
			the embedded script
  --noprogress          Do not show the progress during the decompression
  --nox11               Do not spawn an xterm
  --nochown             Do not give the extracted files to the current user
  --target dir          Extract directly to a target directory
                        directory path can be either absolute or relative
  --tar arg1 [arg2 ...] Access the contents of the archive through the tar command
  --                    Following arguments will be passed to the embedded script
EOH
}

MS_Check()
{
    OLD_PATH="$PATH"
    PATH=${GUESS_MD5_PATH:-"$OLD_PATH:/bin:/usr/bin:/sbin:/usr/local/ssl/bin:/usr/local/bin:/opt/openssl/bin"}
	MD5_ARG=""
    MD5_PATH=`exec <&- 2>&-; which md5sum || command -v md5sum || type md5sum`
    test -x "$MD5_PATH" || MD5_PATH=`exec <&- 2>&-; which md5 || command -v md5 || type md5`
	test -x "$MD5_PATH" || MD5_PATH=`exec <&- 2>&-; which digest || command -v digest || type digest`
    PATH="$OLD_PATH"

    if test x"$quiet" = xn; then
		MS_Printf "Verifying archive integrity..."
    fi
    offset=`head -n 513 "$1" | wc -c | tr -d " "`
    verb=$2
    i=1
    for s in $filesizes
    do
		crc=`echo $CRCsum | cut -d" " -f$i`
		if test -x "$MD5_PATH"; then
			if test x"`basename $MD5_PATH`" = xdigest; then
				MD5_ARG="-a md5"
			fi
			md5=`echo $MD5 | cut -d" " -f$i`
			if test x"$md5" = x00000000000000000000000000000000; then
				test x"$verb" = xy && echo " $1 does not contain an embedded MD5 checksum." >&2
			else
				md5sum=`MS_dd_Progress "$1" $offset $s | eval "$MD5_PATH $MD5_ARG" | cut -b-32`;
				if test x"$md5sum" != x"$md5"; then
					echo "Error in MD5 checksums: $md5sum is different from $md5" >&2
					exit 2
				else
					test x"$verb" = xy && MS_Printf " MD5 checksums are OK." >&2
				fi
				crc="0000000000"; verb=n
			fi
		fi
		if test x"$crc" = x0000000000; then
			test x"$verb" = xy && echo " $1 does not contain a CRC checksum." >&2
		else
			sum1=`MS_dd_Progress "$1" $offset $s | CMD_ENV=xpg4 cksum | awk '{print $1}'`
			if test x"$sum1" = x"$crc"; then
				test x"$verb" = xy && MS_Printf " CRC checksums are OK." >&2
			else
				echo "Error in checksums: $sum1 is different from $crc" >&2
				exit 2;
			fi
		fi
		i=`expr $i + 1`
		offset=`expr $offset + $s`
    done
    if test x"$quiet" = xn; then
		echo " All good."
    fi
}

UnTAR()
{
    if test x"$quiet" = xn; then
		tar $1vf - 2>&1 || { echo Extraction failed. > /dev/tty; kill -15 $$; }
    else

		tar $1f - 2>&1 || { echo Extraction failed. > /dev/tty; kill -15 $$; }
    fi
}

finish=true
xterm_loop=
noprogress=n
nox11=n
copy=none
ownership=y
verbose=n

initargs="$@"

while true
do
    case "$1" in
    -h | --help)
	MS_Help
	exit 0
	;;
    -q | --quiet)
	quiet=y
	noprogress=y
	shift
	;;
    --info)
	echo Identification: "$label"
	echo Target directory: "$targetdir"
	echo Uncompressed size: 24 KB
	echo Compression: gzip
	echo Date of packaging: Thu May 24 08:07:52 CEST 2018
	echo Built with Makeself version 2.2.0 on linux-gnu
	echo Build command was: "./makeself.sh \\
    \"./wismas-bd\" \\
    \"setup-wismas-bd.sh\" \\
    \"Script d'installation by tv (bts sn-ir)\" \\
    \"./wismas-bd.sh\""
	if test x"$script" != x; then
	    echo Script run after extraction:
	    echo "    " $script $scriptargs
	fi
	if test x"" = xcopy; then
		echo "Archive will copy itself to a temporary location"
	fi
	if test x"n" = xy; then
	    echo "directory $targetdir is permanent"
	else
	    echo "$targetdir will be removed after extraction"
	fi
	exit 0
	;;
    --dumpconf)
	echo LABEL=\"$label\"
	echo SCRIPT=\"$script\"
	echo SCRIPTARGS=\"$scriptargs\"
	echo archdirname=\"wismas-bd\"
	echo KEEP=n
	echo NOOVERWRITE=n
	echo COMPRESS=gzip
	echo filesizes=\"$filesizes\"
	echo CRCsum=\"$CRCsum\"
	echo MD5sum=\"$MD5\"
	echo OLDUSIZE=24
	echo OLDSKIP=514
	exit 0
	;;
    --lsm)
cat << EOLSM
No LSM.
EOLSM
	exit 0
	;;
    --list)
	echo Target directory: $targetdir
	offset=`head -n 513 "$0" | wc -c | tr -d " "`
	for s in $filesizes
	do
	    MS_dd "$0" $offset $s | eval "gzip -cd" | UnTAR t
	    offset=`expr $offset + $s`
	done
	exit 0
	;;
	--tar)
	offset=`head -n 513 "$0" | wc -c | tr -d " "`
	arg1="$2"
    if ! shift 2; then MS_Help; exit 1; fi
	for s in $filesizes
	do
	    MS_dd "$0" $offset $s | eval "gzip -cd" | tar "$arg1" - "$@"
	    offset=`expr $offset + $s`
	done
	exit 0
	;;
    --check)
	MS_Check "$0" y
	exit 0
	;;
    --confirm)
	verbose=y
	shift
	;;
	--noexec)
	script=""
	shift
	;;
    --keep)
	keep=y
	shift
	;;
    --target)
	keep=y
	targetdir=${2:-.}
    if ! shift 2; then MS_Help; exit 1; fi
	;;
    --noprogress)
	noprogress=y
	shift
	;;
    --nox11)
	nox11=y
	shift
	;;
    --nochown)
	ownership=n
	shift
	;;
    --xwin)
	if test "n" = n; then
		finish="echo Press Return to close this window...; read junk"
	fi
	xterm_loop=1
	shift
	;;
    --phase2)
	copy=phase2
	shift
	;;
    --)
	shift
	break ;;
    -*)
	echo Unrecognized flag : "$1" >&2
	MS_Help
	exit 1
	;;
    *)
	break ;;
    esac
done

if test x"$quiet" = xy -a x"$verbose" = xy; then
	echo Cannot be verbose and quiet at the same time. >&2
	exit 1
fi

if test x"$copy" \!= xphase2; then
    MS_PrintLicense
fi

case "$copy" in
copy)
    tmpdir=$TMPROOT/makeself.$RANDOM.`date +"%y%m%d%H%M%S"`.$$
    mkdir "$tmpdir" || {
	echo "Could not create temporary directory $tmpdir" >&2
	exit 1
    }
    SCRIPT_COPY="$tmpdir/makeself"
    echo "Copying to a temporary location..." >&2
    cp "$0" "$SCRIPT_COPY"
    chmod +x "$SCRIPT_COPY"
    cd "$TMPROOT"
    exec "$SCRIPT_COPY" --phase2 -- $initargs
    ;;
phase2)
    finish="$finish ; rm -rf `dirname $0`"
    ;;
esac

if test x"$nox11" = xn; then
    if tty -s; then                 # Do we have a terminal?
	:
    else
        if test x"$DISPLAY" != x -a x"$xterm_loop" = x; then  # No, but do we have X?
            if xset q > /dev/null 2>&1; then # Check for valid DISPLAY variable
                GUESS_XTERMS="xterm gnome-terminal rxvt dtterm eterm Eterm xfce4-terminal lxterminal kvt konsole aterm terminology"
                for a in $GUESS_XTERMS; do
                    if type $a >/dev/null 2>&1; then
                        XTERM=$a
                        break
                    fi
                done
                chmod a+x $0 || echo Please add execution rights on $0
                if test `echo "$0" | cut -c1` = "/"; then # Spawn a terminal!
                    exec $XTERM -title "$label" -e "$0" --xwin "$initargs"
                else
                    exec $XTERM -title "$label" -e "./$0" --xwin "$initargs"
                fi
            fi
        fi
    fi
fi

if test x"$targetdir" = x.; then
    tmpdir="."
else
    if test x"$keep" = xy; then
	if test x"$nooverwrite" = xy && test -d "$targetdir"; then
            echo "Target directory $targetdir already exists, aborting." >&2
            exit 1
	fi
	if test x"$quiet" = xn; then
	    echo "Creating directory $targetdir" >&2
	fi
	tmpdir="$targetdir"
	dashp="-p"
    else
	tmpdir="$TMPROOT/selfgz$$$RANDOM"
	dashp=""
    fi
    mkdir $dashp $tmpdir || {
	echo 'Cannot create target directory' $tmpdir >&2
	echo 'You should try option --target dir' >&2
	eval $finish
	exit 1
    }
fi

location="`pwd`"
if test x"$SETUP_NOCHECK" != x1; then
    MS_Check "$0"
fi
offset=`head -n 513 "$0" | wc -c | tr -d " "`

if test x"$verbose" = xy; then
	MS_Printf "About to extract 24 KB in $tmpdir ... Proceed ? [Y/n] "
	read yn
	if test x"$yn" = xn; then
		eval $finish; exit 1
	fi
fi

if test x"$quiet" = xn; then
	MS_Printf "Uncompressing $label"
fi
res=3
if test x"$keep" = xn; then
    trap 'echo Signal caught, cleaning up >&2; cd $TMPROOT; /bin/rm -rf $tmpdir; eval $finish; exit 15' 1 2 3 15
fi

leftspace=`MS_diskspace $tmpdir`
if test -n "$leftspace"; then
    if test "$leftspace" -lt 24; then
        echo
        echo "Not enough space left in "`dirname $tmpdir`" ($leftspace KB) to decompress $0 (24 KB)" >&2
        if test x"$keep" = xn; then
            echo "Consider setting TMPDIR to a directory with more free space."
        fi
        eval $finish; exit 1
    fi
fi

for s in $filesizes
do
    if MS_dd_Progress "$0" $offset $s | eval "gzip -cd" | ( cd "$tmpdir"; umask $ORIG_UMASK ; UnTAR xp ) 1>/dev/null; then
		if test x"$ownership" = xy; then
			(PATH=/usr/xpg4/bin:$PATH; cd "$tmpdir"; chown -R `id -u` .;  chgrp -R `id -g` .)
		fi
    else
		echo >&2
		echo "Unable to decompress $0" >&2
		eval $finish; exit 1
    fi
    offset=`expr $offset + $s`
done
if test x"$quiet" = xn; then
	echo
fi

cd "$tmpdir"
res=0
if test x"$script" != x; then
    if test x"$verbose" = x"y"; then
		MS_Printf "OK to execute: $script $scriptargs $* ? [Y/n] "
		read yn
		if test x"$yn" = x -o x"$yn" = xy -o x"$yn" = xY; then
			eval "\"$script\" $scriptargs \"\$@\""; res=$?;
		fi
    else
		eval "\"$script\" $scriptargs \"\$@\""; res=$?
    fi
    if test "$res" -ne 0; then
		test x"$verbose" = xy && echo "The program '$script' returned an error code ($res)" >&2
    fi
fi
if test x"$keep" = xn; then
    cd $TMPROOT
    /bin/rm -rf $tmpdir
fi
eval $finish; exit $res
‹ ¸V[í\ëvÓÈ“ç+zŠŠ¬CbÆ×\“ŒG	^|alÃÀL¬XíXK2’’…¼Ë~İçø?Í¼ÅVw«u³Ø†9jNˆÔ]]U]]õëê–”BñÁw/%,»¥ş.—Ÿ”·ùï­mVï—å­­Ry«\ÚİÙÁúİí°óào(S×Ó€ŞåítÇºşu¥Püd¸¦ææÏô‚;úó¿õ¤´½Îÿvù6ïîî>€R:ÿß½dÖŠg†U<ÓÜ‘”š=¹“¡µ,l–Ê¥<ş÷Î®áïò™”ÁR&“3]zÑîöZÕ¦Z‘Çö@l×“¥×]µÃ+ÛÆûWÕn÷÷vç°"O4×ıd;º,pŠßëİfµ{ŠRÂŞ‹Š\¥îoXSÁı8–¥Fû8^;¶ÏeªÉ…ëkO=cìJµj¯"³ñ4Ô®«ú·.AÅê­n¯ÚhœÕ¨^qê:¬Í°ĞCÆcÈ›ğd{;${ÕiwªÍù”c8R5`F›òC^{Xï•ºáÈRíÅKZyª¾­w{İO¸äuNA™-"AÎÍ—!_óùB~‚òêfû•`:5_öÔæ«€È#æDTFuå-ÿ¸”•%i8µa[`XqœéÄ;ı85¼,HŸ% P„-@éù’àurY´¦h²ÍgÊÒÃŒlÈ?XunLqÕ,¤"ÖdJtexP–nbR©,.*CİõàOÍ±è@5,Íq]$úåÌ!Ï-â][†ûŒ’¿V»İÓ·=µÓD£^¡ö&èûå\]zp?„İ^Ø–k‰Ì„¼ê¨Gjçômåš¸´Â¿ïõŞ‰š¡í€†š±ºM[±CæğÓÈŒ@ÑĞ¡QÂË}ğFÄòûaa¼*Šæ×z¡Û¡¿_Tß¨LzSo!éQµ¦V(ÉË|Şó®#"¢’câve¿"d‰¤@<ò}²rXï¾jTßÉ°VY†“/ŸF°”Ñä5¬cC:m´Û¯xW%´h’r5±ÂN•X‡(! gyÏğÆ”Àì{ÄÒL¬+e™ß)%”ÏÏ’•çòMÌºXŞ‡Z½ãËÛ³ã9G"VzÚ(¢¯Bt4ÓÖ*JÀ‡º€¯ÊÛl8Ä¤áçØİ·.¹N#t¨œ;Údd|œ°0<tÃØ–q†Ú¸ÔC£W>S¸4t’¥áãÇ1¸Å•ZKx÷•I\W;'Kúx pƒ .¾¨¬‰0±Ö)ìá²>ìşµŠzl%xåÏ¦‡Ñö…v½Wâo:Á¬òB†8L†Útì1:„\‰ò¡µ†ZítkŠçLIX½¿ï_c¼dã:ˆÅhÀzƒ1Ñœ9Ü¨ÑîdÇ©‰«b€‰“î\Äüñ©Wo·*
³Ö¡zT}İèU”Mt·ù8w<¼-tEÁPN8)Z–|¤k°ØG)xß.ZÅ'æ±KQÛÅV‚::ßÑtÈ»”¶øåQ9®·ŞÒXê°(jûáÍïl?°ô
Ç‚Ò¼­X+Ş£<¯Ço±ıá:xU>x_2eV™ğgŸ­0çy›…Õgà°Q±§Æ-FGÄ‹ùvBêC¬Ë]ÈëÇW¢“ˆJõÙ+åPè^9GÓ
¤İÛ”“ä"8•ÃFÌCªNEù5:	J‡™cÓ7:µoiÖ¾9–}œ½‡aÕ£»,‹MËÁ0lPa°–•áˆ¸‡¥½chÂ~:‘ÁE˜H¹d:f0!S­¥S:O¸v¸Ç˜x°Çì1tTâÜ!§†^)mĞN¨Y4Åã9£¦S~e+(®·'¬R˜+„µpÿíŸ÷·ÊûeSítÔ×½ˆt‹øŠyÚQh×º€7°Í	V±!¬‰XŠI‘åıàÅ¿EÃ³ßŠ€cv;_/Æğ÷j§Uoá.åƒä_î1“ûi¬a‹EÌ\³˜FÍB;*;¾]3ià\ÛSÜkóï5Æáµ5Æ˜¢õ`ŸyšaPÑ¡c›˜_âJƒ¤:±3 9FæL-ê}àî÷{(VIªˆçØ¢è‘í¸×<WwÇ0 i€&F#u	Ê¾šx#t/ZcDs— ¹-œÃsÉx¦q>òDÓ1Ëö4ËÃ4Çd,"C+HIë Q4TÀ1µ1LÑ*˜óë8%š‰¹=Õyd°1!ÎH›¸|l¨	R;$!ÚÇ7r¼ è8*EÓr7«¼³6™Œ‹[`IÂ.‚kYçNúvˆ?O{]?”“Í~t±,½ÃyÔÂ¦ç“Ù`|
Ú FâöM§†s=Gól'›“x
fÍâ®„¾g±¸vĞ Ô‘x¿ü'LôâSŒ¾ãWò­3WõXLlÜí1n&¦C8>z}aÙŸ¸û1àğ÷ıY‹cÜIÑ6)&Á×?G]ñOtg$E´0?Şˆ»ïÇ¸Ä¬ëıV¢"¶ÜQ÷À<™uÙhÜ“šàõVüÈ”¤š L÷k°Là(Xr]Bp~ËFæÖ/{+nKø~mŠë$İ¡@Şˆfcù¼æòÓ–»ò¤H>‘H”Q™UeÜêë°¼ğè\‹úF±;İ4Ò(TZ¡ı
½öa[ŠíÅ)N½uú¢İTé¦…~Ät‘h¼İs8yè?kËxĞ-V‰%"Üüÿ¹}‚|4ºš»ÚõéˆŒÇ¶XÌïZÈ¶q!“*÷)ÒÜCÃ§ôÄP3íùĞ!¤0tIR—¯Úúz,úƒóçû‰÷MñÁâÜT»İê±Z‘ßĞ áÆ± ã‹£U ×Œ±4Ò O·-‹ ­ÂO'PÅ$“¯‚®N1¦ùÂ)®°ì(„ÌéñÈ$ØæÑ5J
¡‹8Ub.á+(C)æarSI\Gá©kü7Syˆ>Åú”2s wdÙ}Píªsè(±á³O“Ÿ|Òo‘g£5ºÈşåA¥$Î­ØÎ–]}~|Ã–¤0FÇ$JÎ·¢Veƒ¹•âK+*ŒËPP"u¶u·øş÷{ÚØšš{'
N?ä?OŠÅóõ0›Vçœ 0 Äô<"¸?ÆÜØoúÚ§XÿŒSó«l´ò[Ù›u¬Çdiùr?`kÀ%R?
éGÔîèX„ris»†–¬ºzÂÂˆĞ5pÓâ"êÇ:]ÈRtëw‰ÃĞ…‚ÑĞb­•¾>„üÍê™„ƒœg†M4C?ÔI	ØÙÀ£JåÏ±oHw1'nd9"Í?ÂNÑ	‚àOXÍ¿Û\zPÛ²µ9ÑMLIúWÏ'˜o4£`^èSvT‹}L['ş1EàİÌac^=YõĞéğå$,fÖÌ3‡h³§«áiHÆ?ÁZØ º³
	}¥9ùºïé¨ ÂNëqeÅ w©Õ
ñÃØGeêôña¬ÑŠAŸØx2ş,fŒo3(ÅªHz²)%œ¶-råu=Ìri^Õ‹ŞäXfÕgOÖi
gÒVİF¨Äô1èNìs];_ƒ=¡nPÀôŞCtÇÑ°‡8£˜•ão¦Û  ¿ÈÒä‹DRB-•EGÚ]³ÏÈ@Ã­ôëÈˆé\:»ÿ~föÑhjOÅúÂ*n!f%ÑGÔK‘,…Mûã™‰5ÑÔşäcf8¹c¦…ë­>áş“3t5&lÎäÏj´ÀjcÜ'À]aH÷Vs=˜\apƒ4§ÊšÜÚQ+Ê…b Ydi‘5ef=fxEúp<™GÜC?%¼•Ît±
¬ö´	jÑ6öĞ	-™Ì\h^wkˆ4Apá×RçÑèSÍ¢B;œø{èúÀ_£ŞNÆ¸²™c^»æ§ŠxÂŠó§ˆ§«)âq,ü›ğ3)’¹SÖ~}£}¼ğHW$›OJÇ<ü'ÏML<ß·_Ì¤Áwlb(Ÿè)Ì{~<t2w“1“UÈ¡ÆìlUù\«ön ¬ußÎ£ˆì`2·úòå¾ÆÈÜ9‚{Z'ó¦ÉÌÚ=ã¶fÑ;6··k^W‡`¿GÆ`d á‡bndÀµèãT³¼à\J~–}¬"S„ê<\ ©OWŞ….òdº¸şablC³g<7`8p%+¸É¤î²3«ÄF9fø 1C¯·èú-Óq×!ÑKl¾é±s9™¨«Ãì!ª°&3ÂÚ:	òNz¿6?™Äó‹L|w.ÒHÚ‡PÆß:ˆHŠ,Ò|Y0Ì‰íº,SF|8¶ÏsşšÇ³1£ˆ	Ÿu’äs™ğô!º
ÿ0¾hE Ü„ï@§l³cfÍù‰C­yH7b‰×Hnh‚Šä¨Ò|%G˜(ñEâCƒ5"eö‹[ğpFè|°Zq—a?²–ÂÂ•ØËá›'³DÏ…¯£$3Œ ¿’ıİÎÜH×íA4ÒÛµ["}a€`· Bø
’„,Áó<„ªÀÿ¡*á¡¾+ExbÒG8å}ãwX¯hÕÍ]á÷+Ã;û}Â;Áş[‡÷öñğNİŞwÙ:ÂŸîA¢­s'ÀbV G±¢Y]%+@ê +®CÿÀ¬@Œâ‡€~Ï¬ œÎ‹Oe!‚Ìd>Ä¢‡^P"F1ÁaCT½u=z¼Ûçç¯ÃØ™kîıÅ“ÇJ.·ïæŠü×J®[ïGÙYdYã ÅŒÈÅŞ	`‚l »Ë¾b7¸Ä¶r9˜
ÏU£Ä!}BD}iq–18ËˆG½U_8: à:äñ1ŠBø}·¡áWJ6c^>Ùğ;.›lò•ÉF öû$	öß:ÙXÀ>é	¢•ös£Ü<Eyóx•(o‡Q.®CÿÄ(÷Gñc¢Ü~ï(¿Z”ÇÇ¼B”óKG¹ğ¯r!ö;Eyœı7òùìQ'Z}Kµõì–"h½eK¾^² 9È•çhQìPßö:ÕĞƒÑø¹‹rúbH8’‚"¡øûâHÔü+!ÉÌÈ—Ç’ ë²hñ—¯Ä“ˆèïƒ(3¾5¦,G•²Õq%nõYd‰´/u\1÷8¤Ú@ .<™!KJH–|8›ùÆEÊ<lj†õíùJ¨[†ghcÃe°+!äN_;"¤lÂüå@+ö S’è·‰’¼‡¬ÂWí)ßCbjC_ı¦œcïÆI@ØS¨
òÎf)9*µ'@?£Ö¥ğé6(¼NUĞı'ÑŸOÈ’ø,²Ğ‹eÒñw¤åßY
ÅÄÇÖßéûÿ'ÛÛ‹şşÃöÖ¶øû¥İİ'ìûÿ';é÷ÿGÉç¥|"/¹üç‰»ıˆ[ô‘D’;íW@¡¾Boß—jµÚSÃæú´Ú=ğ×’1ı33u¨Eş…v¦ı»3àâBüWw<ö	kß5<âò1øj¢’9làÕ7ô.Şõ)¾o”ËYF×zİhäh£w=YÔdÙf.5g0ÒœÍH3]ûúˆÆŞœ®´íU§Ş¬vŞÁKõl²RÔÖq½¥Vê–eˆ3¡ö¢Úéª½ÊÔ>EëaÒ¥vzô£ãv8Á%ÇUÎ1õr\‰,¼©6^«İõr©¼[§?op)ÀÙ÷­gÚ·ƒrvÿk¸o2î›øÓ´-Æš5øz®[Œë–àjaÊí3ı~dë–ğ$A(|©Éîgçª¯{íÓzy5ÕV/w§çéšG3zo‘@Êv–{*1qC¢Ñ¡T§Gvµ—‹Õ·ˆqNDã¢M=dä×ûÌ°zjz «šàéÛ}Œµ¨¥,.é\ºä±¼P&ÅÉyõ›j‡ºğÆöá»Ûd øe#©µqSÑ©¢«€oÜÓáÅiÚµ~ÜJ°oôÕVMíó«HS»…!ÔPqêjÕn­z¨.a·º†aÑ¯¨øgdwûGŒZ8I+¿ÒE˜û¸œC÷VÙfP_¹…Æ:{E¿whOé‡|1·JNÓ<9MÑ1şísC–„µC€VÈ	“å’ƒN‚ä!ŸGğŸÿòîo«útŒAXŞ÷İğ§[„Ğü™Z¤¨zSİĞ¡Êå
X¥$~e7…²OG[%¦î¼ˆ(¼µ„Â¥y
?ı.ún	}Ë%aİ˜º¥»Õ}:OÛMÔ6ÍğÓ’–´¤%-iIKZÒ’–´¤%-iIKZÒ’–´¤%-iIKZÒ’–´¤%-iIKZÒ’–´¤%-iIKZÒòo*ÿû`ÜY x  