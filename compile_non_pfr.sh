#! /bin/bash

set -e

# Run prep_revisions.sh to generate all files
if [ ! -f "prep_done.txt" ]; then
	./prep_revisions.sh
fi

set UNAME ""
UNAME=$(uname -r)

case "$UNAME" in
	*Microsoft*) WINDOWS_EXE=.exe;;
	*) WINDOWS_EXE=;;
esac

quartus_map$WINDOWS_EXE --write_settings_files=off top -c non_pfr
quartus_cdb$WINDOWS_EXE --write_settings_files=off --merge top -c non_pfr
quartus_fit$WINDOWS_EXE --write_settings_files=off top -c non_pfr
quartus_asm$WINDOWS_EXE --write_settings_files=off top -c non_pfr
quartus_sta$WINDOWS_EXE top -c non_pfr
quartus_pow$WINDOWS_EXE --write_settings_files=off top -c non_pfr
quartus_cpf$WINDOWS_EXE -c non_pfr.cof

echo "Compilation complete"