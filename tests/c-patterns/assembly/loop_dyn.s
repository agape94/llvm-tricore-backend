	.text
	.file	"loop_dyn.c"
	.globl	main                            # -- Begin function main
	.type	main,@function
main:                                   # @main
.Lfunc_begin0:
	.file	0 "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns" "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns/src/loop_dyn.c" md5 0x022e4eb4b299afdc28b8a26f1d313c8b
	.file	1 "src" "loop_dyn.c" md5 0x022e4eb4b299afdc28b8a26f1d313c8b
	.loc	1 1 0                           # src/loop_dyn.c:1:0
# %bb.0:                                # %entry
	mov.aa %a14, %a10
	sub.a %a10, 24
	mov %d15, 0
	st.w [%a14] -4, %d15
	st.w [%a14] -8, %d4
	mov.a %d2, %a4
	st.w [%a14] -12, %d2
.Ltmp0:
	.loc	1 2 7 prologue_end              # src/loop_dyn.c:2:7
	st.w [%a14] -16, %d15
	.loc	1 4 7                           # src/loop_dyn.c:4:7
	st.w [%a14] -24, %d15
	.loc	1 6 3                           # src/loop_dyn.c:6:3
	j .LBB0_1
.LBB0_1:                                # %while.cond
                                        # =>This Inner Loop Header: Depth=1
	.loc	1 6 9 is_stmt 0                 # src/loop_dyn.c:6:9
	ld.w %d15, [%a14] -24
	.loc	1 6 13                          # src/loop_dyn.c:6:13
	ld.w %d2, [%a14] -20
	.loc	1 6 3                           # src/loop_dyn.c:6:3
	ge %d15, %d15, %d2
	jnz %d15, .LBB0_3
	j .LBB0_2
.LBB0_2:                                # %while.body
                                        #   in Loop: Header=BB0_1 Depth=1
.Ltmp1:
	.loc	1 7 7 is_stmt 1                 # src/loop_dyn.c:7:7
	ld.w %d2, [%a14] -16
	mov %d15, 2
	add %d15, %d2
	st.w [%a14] -16, %d15
	.loc	1 8 6                           # src/loop_dyn.c:8:6
	ld.w %d2, [%a14] -24
	mov %d15, 1
	add %d15, %d2
	st.w [%a14] -24, %d15
.Ltmp2:
	.loc	1 6 3                           # src/loop_dyn.c:6:3
	j .LBB0_1
.LBB0_3:                                # %while.end
	.loc	1 10 10                         # src/loop_dyn.c:10:10
	ld.w %d2, [%a14] -16
	.loc	1 10 3 is_stmt 0                # src/loop_dyn.c:10:3
	ret
.Ltmp3:
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
	.byte	39                              # DW_AT_prototyped
	.byte	25                              # DW_FORM_flag_present
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	63                              # DW_AT_external
	.byte	25                              # DW_FORM_flag_present
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	3                               # Abbreviation Code
	.byte	5                               # DW_TAG_formal_parameter
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
	.byte	5                               # Abbreviation Code
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
	.byte	6                               # Abbreviation Code
	.byte	15                              # DW_TAG_pointer_type
	.byte	0                               # DW_CHILDREN_no
	.byte	73                              # DW_AT_type
	.byte	19                              # DW_FORM_ref4
	.byte	0                               # EOM(1)
	.byte	0                               # EOM(2)
	.byte	7                               # Abbreviation Code
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
	.byte	1                               # Abbrev [1] 0xc:0x76 DW_TAG_compile_unit
	.byte	0                               # DW_AT_producer
	.short	29                              # DW_AT_language
	.byte	1                               # DW_AT_name
	.word	.Lstr_offsets_base0             # DW_AT_str_offsets_base
	.word	.Lline_table_start0             # DW_AT_stmt_list
	.byte	2                               # DW_AT_comp_dir
	.byte	0                               # DW_AT_low_pc
	.word	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.word	.Laddr_table_base0              # DW_AT_addr_base
	.byte	2                               # Abbrev [2] 0x23:0x47 DW_TAG_subprogram
	.byte	0                               # DW_AT_low_pc
	.word	.Lfunc_end0-.Lfunc_begin0       # DW_AT_high_pc
	.byte	1                               # DW_AT_frame_base
	.byte	110
	.byte	3                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
                                        # DW_AT_prototyped
	.word	106                             # DW_AT_type
                                        # DW_AT_external
	.byte	3                               # Abbrev [3] 0x32:0xb DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	16
	.byte	5                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
	.word	106                             # DW_AT_type
	.byte	3                               # Abbrev [3] 0x3d:0xb DW_TAG_formal_parameter
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	12
	.byte	6                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	1                               # DW_AT_decl_line
	.word	110                             # DW_AT_type
	.byte	4                               # Abbrev [4] 0x48:0xb DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	8
	.byte	8                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	2                               # DW_AT_decl_line
	.word	106                             # DW_AT_type
	.byte	4                               # Abbrev [4] 0x53:0xb DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	4
	.byte	9                               # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	3                               # DW_AT_decl_line
	.word	124                             # DW_AT_type
	.byte	4                               # Abbrev [4] 0x5e:0xb DW_TAG_variable
	.byte	2                               # DW_AT_location
	.byte	145
	.byte	0
	.byte	10                              # DW_AT_name
	.byte	1                               # DW_AT_decl_file
	.byte	4                               # DW_AT_decl_line
	.word	106                             # DW_AT_type
	.byte	0                               # End Of Children Mark
	.byte	5                               # Abbrev [5] 0x6a:0x4 DW_TAG_base_type
	.byte	4                               # DW_AT_name
	.byte	5                               # DW_AT_encoding
	.byte	4                               # DW_AT_byte_size
	.byte	6                               # Abbrev [6] 0x6e:0x5 DW_TAG_pointer_type
	.word	115                             # DW_AT_type
	.byte	6                               # Abbrev [6] 0x73:0x5 DW_TAG_pointer_type
	.word	120                             # DW_AT_type
	.byte	5                               # Abbrev [5] 0x78:0x4 DW_TAG_base_type
	.byte	7                               # DW_AT_name
	.byte	6                               # DW_AT_encoding
	.byte	1                               # DW_AT_byte_size
	.byte	7                               # Abbrev [7] 0x7c:0x5 DW_TAG_volatile_type
	.word	106                             # DW_AT_type
	.byte	0                               # End Of Children Mark
.Ldebug_info_end0:
	.section	.debug_str_offsets,"",@progbits
	.word	48                              # Length of String Offsets Set
	.short	5
	.short	0
.Lstr_offsets_base0:
	.section	.debug_str,"MS",@progbits,1
.Linfo_string0:
	.string "clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git c9cb984bc8a2f90da0438086ef150e22d1e874be)" # string offset=0
.Linfo_string1:
	.string "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns/src/loop_dyn.c" # string offset=127
.Linfo_string2:
	.string "/home/alex/tugraz/thesis/llvm-tricore-backend/tests/c-patterns" # string offset=205
.Linfo_string3:
	.string "main"                          # string offset=268
.Linfo_string4:
	.string "int"                           # string offset=273
.Linfo_string5:
	.string "argc"                          # string offset=277
.Linfo_string6:
	.string "argv"                          # string offset=282
.Linfo_string7:
	.string "char"                          # string offset=287
.Linfo_string8:
	.string "a"                             # string offset=292
.Linfo_string9:
	.string "num"                           # string offset=294
.Linfo_string10:
	.string "i"                             # string offset=298
	.section	.debug_str_offsets,"",@progbits
	.word	.Linfo_string0
	.word	.Linfo_string1
	.word	.Linfo_string2
	.word	.Linfo_string3
	.word	.Linfo_string4
	.word	.Linfo_string5
	.word	.Linfo_string6
	.word	.Linfo_string7
	.word	.Linfo_string8
	.word	.Linfo_string9
	.word	.Linfo_string10
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
