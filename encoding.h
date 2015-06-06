#pragma once

UINT surrogate_to_ucs(const UINT16 _srch, const UINT16 _srcl);
bool is_dblchar(UINT n);
bool is_dblchar_cjk(UINT n);

//EOF
