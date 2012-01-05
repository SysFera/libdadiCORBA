## generate doxygen documentation
add_custom_target(doc ALL)
if(ENABLE_DOXYGEN)
  include(UseDoxygen)
  generate_doxygen(FORMATS ${DOXYGEN_FORMATS})
endif()

## generate extras documentation (specifications)
if(ENABLE_EXTRAS_DOC)
  find_package(LATEX)
  if(PDFLATEX_COMPILER)
    include(UseLATEX)
    add_latex_document(
      "UsersManual.tex" 
      IMAGE_DIRS fig images
      MANGLE_TARGET_NAMES
      DEFAULT_PDF)
    add_custom_target(pdf)
    add_dependencies(pdf authentification-specifications_pdf)
    add_dependencies(doc pdf)
  endif()
endif()
