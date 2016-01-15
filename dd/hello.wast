(module
  (memory 31 4294967295 (segment 16 "Hello, world!\n\00"))
  (import $puts "env" "puts" (param i32))
  (export "main" $main)
  (func $main (result i32)
    (block $fake_return_waka123
      (block
        (call_import $puts
          (i32.const 16)
        )
        (br $fake_return_waka123
          (i32.const 0)
        )
      )
    )
  )
)
;; METADATA: { "asmConsts": {},"staticBump": 30 }
