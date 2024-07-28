; ModuleID = './tests/basic-bench.c'
source_filename = "./tests/basic-bench.c"
target datalayout = "e-m:e-p:32:32-i64:32-a:0:32-n32"
target triple = "tricore"

%struct.bench = type { i16, i16, i16, i16 }

@global_value = dso_local global i64 -1, align 8
@l = dso_local global i32 19088743, align 4
@ll = dso_local global i64 1250999861248, align 8
@s = dso_local global i16 4660, align 2
@c = dso_local global i8 18, align 1
@scratch = dso_local global i64 0, align 8
@global_shift = dso_local global i32 0, align 4

; Function Attrs: noinline nounwind optnone
define dso_local void @bench_start(ptr inreg noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr %2, align 4
  %4 = getelementptr inbounds %struct.bench, ptr %3, i32 0, i32 0
  store i16 0, ptr %4, align 2
  ret void
}

; Function Attrs: noinline nounwind optnone
define dso_local void @bench_stop(ptr inreg noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr %2, align 4
  %4 = getelementptr inbounds %struct.bench, ptr %3, i32 0, i32 1
  store i16 100, ptr %4, align 2
  ret void
}

; Function Attrs: noinline nounwind optnone
define dso_local void @bench_bits(ptr inreg noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  store ptr null, ptr %2, align 4
  ret void
}

; Function Attrs: noinline nounwind optnone
define dso_local i32 @main() #0 {
  %1 = alloca i32, align 4
  %2 = alloca %struct.bench, align 2
  store i32 0, ptr %1, align 4
  call void @bench_bits(ptr inreg noundef %2)
  ret i32 0
}

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" }

!llvm.module.flags = !{!0, !1}
!llvm.ident = !{!2}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 7, !"frame-pointer", i32 2}
!2 = !{!"clang version 17.0.6 (git@gitlab.tugraz.at:D2269AF97C2F43AC/llvm-tricore-backend.git bcb674e77aecd1469dacdc839de96f875afc2ca0)"}
