OpenCppCoverage --excluded_line_regex "\s*[\{\}].*" --sources=Utils\* --sources=ShaderManager\* --sources=Lust\* --export_type=binary:MathTests.cov  -- .\Windows\Debug\bin\MathTests.exe
OpenCppCoverage --excluded_line_regex "\s*[\{\}].*" --sources=Utils\* --sources=ShaderManager\* --sources=Lust\* --export_type=binary:LustUtilsTest.cov -- .\Windows\Debug\bin\LustUtilsTest.exe 
OpenCppCoverage --excluded_line_regex "\s*[\{\}].*" --sources=Utils\* --sources=ShaderManager\* --sources=Lust\* --export_type=binary:LustShaderManagerTest.cov -- .\Windows\Debug\bin\LustShaderManagerTest.exe
OpenCppCoverage.exe --input_coverage=MathTests.cov --input_coverage=LustUtilsTest.cov --input_coverage=LustShaderManagerTest.cov --export_type=html:final_coverage
