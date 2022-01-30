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
� �V[�\�v�ȓ�+z���Cb��\��G	^|al���L�X�XK2�����~���?ͼ�Vw�u�����9jN��]]U]]��ꖔB��w/%,���.������mV�孭Ry�\�����ݝ����o(S������t���u�P�d������;��������v�6���>�R:�߽d֊g�U<�ܑ���=����,l�ʥ<��ή����R&��3]z���ZզZ���@�lד��]��+���W�n��v�"O4��d;�,p����f�{��R�ދ�\���o�XS��8��F�8^;��e�ɝ��kO=c�J�j�"��4Ԯ���.A��n��h���^q�:�Ͱ�C�cț�d{;${�iw����c8R5`F��C^{X�����R��KZy���w{�O���uNA�-"A�͗!_��B~���f��`:5_��櫀�#�DTFu�-�����%i8��a[`Xq���;�85��,H�% P�-@����urY��h��g�����l��?XunL�q�,�"�dJtexP�nbR�,.*C���Oͱ�@5,�q]$���!�-�][�����V��ӷ=��D�^��&���\]zp�?��^ؖk��̄��Gj��m嚸�¿��މ��퀆����M[�C�����@���Q��}�F���aa�*���z����_TߨLzSo!�Q��V(��|��#"��c�ve�"d��@<�}�rX�jT�ɰVY��/�F�����5�cC:m�ۯxW%�h�r5��N�X�(! gy�������{��L�+e��)%�������M̺XއZ���۳�9�G�"Vz�(���Bt�4��*J�������l8Ĥ���ݷ�.�N#t��;�dd|��0<tÝؖq�ڸ�C�W>S�4t�����1�ŕZKx��I\W;'K�x�p� .����0��)��>����zl%x�Ϧ�����v�W��o:���B��8L��t�1:�\�����Z�tk���LIX���_c�d�:��h��z�1ќ9ܨ��dǩ��b����\���Wo�*
�֡zT}��U�Mt���8w<�-tE�PN8)Z�|�k��G)x�.Zŏ'�KQ��V�::��tȻ����Q9����X�(j����l?��
ǂҼ�X+ޣ<��o���:xU>x_2eV��g��0�y���g�Q���-FGċ�vB�C��]���W���J��+�P�^9G�
��۔��"8��F�C�NE�5:	J��c�7:�oi־�9�}���aգ�,�M��0lPa���������ch�~:��E�H�d:f0!S���S:O�v�ǘx���1tT��!���^)m�N�Y4��9��S~e+(��'�R�+��p������eS�t�ם��t���y�Qhמ��7��	V�!��X�I�������Eóߊ�cv;_/���j�Uo�.��_�1��i�a��E�\��F�B;*;�]3i�\�S�k��5��5Ƙ��`�y�aPѡc��_�J��:�3 9F�L-�}���{(VI��������<Ww�0 �i��&F#u	���x#t/ZcDs���-��s�x�q>�D�1��4�Á�4�d,"C+HI�Q4T�1�1L�*���8%���=�yd�1!�H��|l�	R;$!��7r� �8*E�r7���6����[`I��.�kY�N�v�?O{]?���~t�,��y�¦��`|
ڠF��M��s=G�l'��x
f�⮄�g��v� ԑx��'L��S���W�3W�XLl��1n&�C8>z}aٟ��1����Y�c�I�6)&��?G]�Otg$E�0�?ވ���ǸĬ��V�"��Q��<�u�h�����V�Ȕ�� L�k�L�(Xr]Bp~�F��/{+nK�~m��$ݡ@ވfc����Ӗ��H>�H�Q�Ue��밼��\��F�;�4�(TZ��
��a[���)N�u���T馝�~�t�h��s8y�?k�x�-V�%"����}�|4�����鈌ǶX��Zȶq!�*�)��Cç��P3���!�0t�IR����z,�������M����T���Z��� ��� ㋣U ׌�4� O�-� ��O'P�$�����N1���)���(�����$���5J
��8Ub.�+(C)�ar�S�I\G��k�7Sy�>���2s wd�}P�s�(��O��|�o�g�5����A�$έ�Ζ]}~|Ï��0F�$Jη�Ve����K+*��PP"u�u�����{�ؚ�{'
N?�?O����0�V眠0 ��<"�?Ɲ��o�ڧX��S��l���[ٛu��di�r?`k��%R?�
�G����X�ris�����z��5p��"��:]�Rt�w��Ѕ���b���>�������g�M4C?�I	����J�ϱoHw1'�nd9"�?�N�	��OX���\zP۲�9�MLI�W�'�o4�`^��SvT�}L['�1E���ac^=Y�����$,f��3�h����iH�?�Z� ��
	}�9���騠�N�qe� w��
���Ge���a�с�A��x2�,f�o�3(�ŪHz�)%���-r�u=�ri^՞���Xf�gO�i
g�V�F���1�N�s];_�=�nP���Ct�Ѱ�8����o�۠�����DR�B-�EG�]���@í��Ȉ�\:��~f��hjO���*n!f%�G�K�,�M�㙉5����cf8�c���>���3t5&l���j��jc�'�]aH�Vs=�\ap�4�ʚ��Q+ʅb Ydi�5ef=fxE�p<���G�C?%���t��
���	j�6��	-��\h^wk�4A�p��R����S͢B;��{���_��NƸ��c^�槊x󧈧��)�q,���3)��S�~}�}��HW$�OJ�<�'�ML<߷_�̤�wlb(��)�{~<t2w�1�Uȡ��lU�\��n ��u�Σ��`2�������9�{Z'�����=�f�;6��k^W�`�G�`d ��bnd����T����\J~�}�"S��<\��OWޅ.�d���ablC�g<7`8p%+�ɤ�3���F9f� 1C����-�q�!�Kl��s9�����!��&3��:	�Nz�6?���L|w.�HڇP��:��H�,�|Y0̉�,SF|8��s��ǳ1��	�u��s���!�
�0�hE ܞ��@�l�cf����C�yH7b��Hnh����|%G�(�E�C�5"e��[�pF�|�Zq�a?������'�DϞ���$3� ������H��A4�۵["}a�`� B�
��,��<�����*᡾+Exb�G8�}�w�X�h��]��+�;�}�;��[�����N��w�:�A��s'�bV�G��Y]%+@� +�C���@�⇀�~Ϭ �΋O�e!��d��>Ģ�^P"F1�aCT�u=z�����ؙk��œ�J.�����J��[�G�YdY�Ō���	`�l ���b7�Ķr9�
�U��!}BD}iq�18ˈG�U_8�: ��:��1�B�}���WJ6c^>��;.�l��F ��$	��:�X�>�	���s��<�Ey�x�(o�Q.�C��(�G�c��~�(�Z��ǼB��KG�𐯍r!�;Ey��7����Q'Z}K���"h�eK�^� 9ȕ�hQ�P��:�Ѓ����r�bH8��"����H��+!��ȗǒ��h�ē���(3�5�,G���q%n�Yd��/u\1�8���@ .<�!KJH�|8���E�<lj����J�[�ghc�e�+!�N_;"�l���@+� �S�跉������W�)�Cbj�C_���c��I@�S�
��f)9*�'@�?�֥��6(�NU��'џOȒ�,��Џ�e��w���Y
��������'�ۋ����ֶ����ݝ'���';���G��|"/�������[��D�;�W@���Boߗj��S�����=�ג1�33u�E���v����3��B�Ww<�	k�5<��1�j���9l��7�.��)�o��YF�z�h�h�w=Y�d�f.5g0Ҝ�͝�H3]����ޜ���U�ެv��K�l�R��q��V�e��3����骽��>E�aҥvz���v8�%�U�1�r\�,��6^�ݍ�r���[�?op)����gڎ���rv�k�o2��Ӵ-ƚ5�z�[���ja��3��~d��$A(|���g���{��zy5�V/w���G3zo�@�v�{*1qC�ѡT�G�v���շ�qND�M=d���̰zjz������}����,.�\����P&��y��j�������d �e#��qSѩ���o����i���~�J�o��VM��HS��!�Pq�j�n�z�.a���aѯ��gdw�G�Z8I+��E����C�VَfP_���:{E�whO�|1�JN�<9M�1��sC���C�V�	�咃N��!��G�����o��t�AX����[����Z��zS�����
X�$~e7��OG[%����(���¥y
?�.�n	}�%aݘ����}:O�M�6��Ӓ���%-iIKZҒ���%-iIKZҒ���%-iIKZҒ���%-iIKZҒ���%-iIKZ��o*��`�Y x  