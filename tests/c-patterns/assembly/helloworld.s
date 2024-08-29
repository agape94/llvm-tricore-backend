	.text
	.file	"helloworld.c"
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
.Lfunc_begin0:
	.file	0 "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns" "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns/src/helloworld.c" md5 0xdf02764e4dc6c61381391ebb750cf94d
	.file	1 "src" "helloworld.c" md5 0xdf02764e4dc6c61381391ebb750cf94d
	.loc	1 4 0                           # src/helloworld.c:4:0
# %bb.0:                                # %entry
	mov.aa %a14, %a10
	sub.a %a10, 8
	mov %d15, 0
	st.w [%a14] -4, %d15
	mov %d15, 10
.Ltmp0:
	.loc	1 5 16 prologue_end             # src/helloworld.c:5:16
	st.w [%a14] -8, %d15
.Ltmp1:
	.loc	1 6 7                           # src/helloworld.c:6:7
	ld.w %d15, [%a14] -8
.Ltmp2:
	.loc	1 6 7 is_stmt 0                 # src/helloworld.c:6:7
	lt %d15, %d15, 11
	jnz %d15, .LBB0_2
	j .LBB0_1
.LBB0_1:                                # %if.then
	.loc	1 0 7                           # src/helloworld.c:0:7
	mov %d15, 100
.Ltmp3:
	.loc	1 8 5 is_stmt 1                 # src/helloworld.c:8:5
	st.w [%a14] -4, %d15
	j .LBB0_3
.LBB0_2:                                # %if.else
	.loc	1 0 5 is_stmt 0                 # src/helloworld.c:0:5
	mov %d15, 200
	.loc	1 11 5 is_stmt 1                # src/helloworld.c:11:5
	st.w [%a14] -4, %d15
	j .LBB0_3
.Ltmp4:
.LBB0_3:                                # %return
	.loc	1 13 1                          # src/helloworld.c:13:1
	ld.w %d2, [%a14] -4
	ret
.Ltmp5:
.Lfunc_end0:
	.size	main, .Lfunc_end0-main
                                        # -- End function
	.section	.debug_abbrev,"",@progbits
	.byte	1                               # Abbreviation Code
	.byte	17                              # DW_TAG_compile_unit
	.byte	1                               # DW_CHILDREN_yes
	.byte	37                              # DW_AT_producer
	.byte	37                              # DW_FORM_strx1
	.byte	19                              # DW_AT_language
	.byte	5                               # DW_FORM_data2
	.byte	3                               # DW_AT_name
	.byte	37                              # DW_FORM_strx1
	.byte	114                             # DW_AT_str_offsets_base
	.byte	23                              # DW_FORM_sec_offset
	.byte	16                              # DW_AT_stmt_list
	.byte	23                              # DW_FORM_sec_offset
	.byte	27                              # DW_AT_comp_dir
	.byte	37                              # DW_FORM_strx1
	.byte	17                              # DW_AT_low_pc
	.byte	27                              # DW_FORM_addrx
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	115                             # DW_AT_addr_base
	.byte	23                              # DW_FORM_sec_offset
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	2                               # Abbreviation Code
	.byte	46                              # DW_TAG_subprogram
	.byte	1                               # DW_CHILDREN_yes
	.byte	17                              # DW_AT_low_pc
	.byte	27                              # DW_FORM_addrx
	.byte	18                              # DW_AT_high_pc
	.byte	6                               # DW_FORM_data4
	.byte	64                              # DW_AT_frame_base
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	37                              # DW_FORM_strx1
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	3                               # Abbreviation Code
	.byte	52                              # DW_TAG_variable
	.byte	0                               # DW_CHILDREN_no
	.byte	2                               # DW_AT_location
	.byte	24                              # DW_FORM_exprloc
	.byte	3                               # DW_AT_name
	.byte	37                              # DW_FORM_strx1
	.byte	58                              # DW_AT_decl_file
	.byte	11                              # DW_FORM_data1
	.byte	59                              # DW_AT_decl_line
	.byte	11                              # DW_FORM_data1
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	4                               # Abbreviation Code
	.byte	36                              # DW_TAG_base_type
	.byte	0                               # DW_CHILDREN_no
	.byte	3                               # DW_AT_name
	.byte	37                              # DW_FORM_strx1
	.byte	62                              # DW_AT_encoding
	.byte	11                              # DW_FORM_data1
	.byte	11                              # DW_AT_byte_size
	.byte	11                              # DW_FORM_data1
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	5                               # Abbreviation Code
	.byte	53                              # DW_TAG_volatile_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	0                               # EOM(3)
	.section	.debug_info,"",@progbits
.Lcu_begin0:
	.word	.Ldebug_info_end0-.Ldebug_info_start0 # Length of Unit
.Ldebug_info_start0:
	.short	5                               # DWARF version number
	.byte	1                               # DWARF Unit Type
	.byte	4                               # Address Size (in bytes)
	.word	.debug_abbrev                   # Offset Into Abbrev. Section
	.byte	1                               # Abbrev [1] 0xc:0x3c DW_TAG_compile_unit
	.byte	0                               # DW_AT_producer
	.short	29                              # DW_AT_language
	.byte	1                               # DW_AT_name
	.word	.Lstr_offsets_base0             # DW_AT_str_offsets_base
	.word	.Lline_table_start0             # DW_AT_stmt_list
	.byte	2                               # DW_AT_comp_dir
	.byte	0                               # DW_AT_low_pc
	.word	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.word	.Laddr_table_base0              # DW_AT_addr_base
	.byte	2                               # Abbrev [2] 0x23:0x1b DW_TAG_subprogram
	.byte	0                               # DW_AT_low_pc
	.word	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	110
	.byte	3                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.word	62                              # DW_AT_type
                                        # DW_AT_external
	.byte	3                               # Abbrev [3] 0x32:0xb DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	0
	.byte	5                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	5                               # DW_AT_decl_line
	.word	66                              # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	4                               # Abbrev [4] 0x3e:0x4 DW_TAG_base_type
	.byte	4                               # DW_AT_name
	.byte	5                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	5                               # Abbrev [5] 0x42:0x5 DW_TAG_volatile_type
	.word	62                              # DW_AT_type
	.byte	0                               # End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str_offsets,"",@progbits
	.word	28                              # Length of String Offsets Set
	.short	5
	.short	0
.Lstr_offsets_base0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.string "clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)" # string offset=0
.Linfo_string1:
	.string "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns/src/helloworld.c" # string offset=127
.Linfo_string2:
	.string "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns" # string offset=207
.Linfo_string3:
	.string "main"                          # string offset=270
.Linfo_string4:
	.string "int"                           # string offset=275
.Linfo_string5:
	.string "a"                             # string offset=279
	.section	.debug_str_offsets,"",@progbits
	.word	.Linfo_string0
	.word	.Linfo_string1
	.word	.Linfo_string2
	.word	.Linfo_string3
	.word	.Linfo_string4
	.word	.Linfo_string5
	.section	.debug_addr,"",@progbits
	.word	.Ldebug_addr_end0-.Ldebug_addr_start0 # Length of contribution
.Ldebug_addr_start0:
	.short	5                               # DWARF version number
	.byte	4                               # Address size
	.byte	0                               # Segment selector size
.Laddr_table_base0:
	.word	.Lfunc_begin0
.Ldebug_addr_end0:
	.ident	"clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)"
	.section	".note.GNU-stack","",@progbits
	.addrsig
	.section	.debug_line,"",@progbits
.Lline_table_start0:
