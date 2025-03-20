OpenCppCoverage --sources Utils\* --sources ShaderManager\* --sources Lust\* --export_type=binary:MathTests.cov  -- .\Windows\Debug\bin\MathTests.exe
OpenCppCoverage --sources Utils\* --sources ShaderManager\* --sources Lust\* --export_type=binary:LustUtilsTest.cov -- .\Windows\Debug\bin\LustUtilsTest.exe 
OpenCppCoverage.exe --input_coverage=MathTests.cov --input_coverage=LustUtilsTest.cov --export_type=html:final_coverage
