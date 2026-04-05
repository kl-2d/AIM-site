QT += gui widgets sql

CONFIG += c++17
CONFIG -= app_bundle


# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

TRANSLATIONS += translations_ru.ts
CONFIG += lrelease embed_translations

SOURCES += \
        boolstatus.cpp \
        cachecontroller.cpp \
        columntype.cpp \
        csvfileprocessor.cpp \
        databasecontroller.cpp \
        datacolumnsmodel.cpp \
        datamodel.cpp \
        filecolumnsmodel.cpp \
        filternamesstorage.cpp \
        main.cpp \
        # miniz/miniz.c \
        # miniz/miniz_tdef.c \
        # miniz/miniz_tinfl.c \
        # miniz/miniz_zip.c \
        # odr/document.cpp \
        # odr/document_cursor.cpp \
        # odr/document_element.cpp \
        # odr/exceptions.cpp \
        # odr/file.cpp \
        # odr/html.cpp \
        # odr/internal/cfb/cfb_archive.cpp \
        # odr/internal/cfb/cfb_impl.cpp \
        # odr/internal/cfb/cfb_util.cpp \
        # odr/internal/common/document_cursor.cpp \
        # odr/internal/common/document_path.cpp \
        # odr/internal/common/file.cpp \
        # odr/internal/common/filesystem.cpp \
        # odr/internal/common/image_file.cpp \
        # odr/internal/common/path.cpp \
        # odr/internal/common/random.cpp \
        # odr/internal/common/style.cpp \
        # odr/internal/common/table_cursor.cpp \
        # odr/internal/common/table_position.cpp \
        # odr/internal/common/table_range.cpp \
        # odr/internal/common/temporary_file.cpp \
        # odr/internal/crypto/crypto_util.cpp \
        # odr/internal/git_info.cpp \
        # odr/internal/html/common.cpp \
        # odr/internal/html/css_writer.cpp \
        # odr/internal/html/document.cpp \
        # odr/internal/html/document_element.cpp \
        # odr/internal/html/document_style.cpp \
        # odr/internal/html/image_file.cpp \
        # odr/internal/html/text_file.cpp \
        # odr/internal/magic.cpp \
        # odr/internal/odf/odf_crypto.cpp \
        # odr/internal/odf/odf_cursor.cpp \
        # odr/internal/odf/odf_document.cpp \
        # odr/internal/odf/odf_element.cpp \
        # odr/internal/odf/odf_file.cpp \
        # odr/internal/odf/odf_manifest.cpp \
        # odr/internal/odf/odf_meta.cpp \
        # odr/internal/odf/odf_style.cpp \
        # odr/internal/oldms/oldms_file.cpp \
        # odr/internal/ooxml/ooxml_crypto.cpp \
        # odr/internal/ooxml/ooxml_file.cpp \
        # odr/internal/ooxml/ooxml_meta.cpp \
        # odr/internal/ooxml/ooxml_util.cpp \
        # odr/internal/ooxml/presentation/ooxml_presentation_cursor.cpp \
        # odr/internal/ooxml/presentation/ooxml_presentation_document.cpp \
        # odr/internal/ooxml/presentation/ooxml_presentation_element.cpp \
        # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_cursor.cpp \
        # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_document.cpp \
        # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_element.cpp \
        # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_style.cpp \
        # odr/internal/ooxml/text/ooxml_text_cursor.cpp \
        # odr/internal/ooxml/text/ooxml_text_document.cpp \
        # odr/internal/ooxml/text/ooxml_text_element.cpp \
        # odr/internal/ooxml/text/ooxml_text_style.cpp \
        # odr/internal/open_strategy.cpp \
        # odr/internal/project_info.cpp \
        # odr/internal/resource.cpp \
        # odr/internal/resource_data.cpp \
        # odr/internal/svm/svm_file.cpp \
        # odr/internal/svm/svm_format.cpp \
        # odr/internal/svm/svm_to_svg.cpp \
        # odr/internal/util/file_util.cpp \
        # odr/internal/util/hash_util.cpp \
        # odr/internal/util/stream_util.cpp \
        # odr/internal/util/string_util.cpp \
        # odr/internal/util/xml_util.cpp \
        # odr/internal/zip/zip_archive.cpp \
        # odr/internal/zip/zip_util.cpp \
        # odr/open_document_reader.cpp \
        # odr/quantity.cpp \
        # odr/style.cpp \
        reportgenerator.cpp \
        tablemodel.cpp \
        tableproxymodel.cpp \
        ui/behaviortype.cpp \
        ui/boolfilterwidget.cpp \
        ui/collapsiblewidget.cpp \
        ui/columnsettingswidget.cpp \
        ui/datetimefilterwidget.cpp \
        ui/filterswidget.cpp \
        ui/filtervalueedit.cpp \
        ui/filterwidget.cpp \
        ui/horizontalseparator.cpp \
        ui/intcolumnsettingswidget.cpp \
        ui/mainwindow.cpp \
        ui/numberfilterwidget.cpp \
        ui/reportsettingswidget.cpp \
        ui/stringcolumnsettingswidget.cpp \
        ui/stringfilterwidget.cpp \
        ui/stringvaluesfilterwidget.cpp \
        ui/tableheaderpreviewwidget.cpp \
        ui/tablesettingswidget.cpp \
        ui/tablewidget.cpp \
        ui/verticalseparator.cpp \
        utils.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    boolstatus.h \
    cachecontroller.h \
    columntype.h \
    csvfileprocessor.h \
    databasecontroller.h \
    datacolumnsmodel.h \
    datamodel.h \
    filecolumnsmodel.h \
    filternamesstorage.h \
    filterparams.h \
    # miniz/miniz.h \
    # miniz/miniz_common.h \
    # miniz/miniz_export.h \
    # miniz/miniz_tdef.h \
    # miniz/miniz_tinfl.h \
    # miniz/miniz_zip.h \
    # odr/document.h \
    # odr/document_cursor.h \
    # odr/document_element.h \
    # odr/exceptions.h \
    # odr/file.h \
    # odr/html.h \
    # odr/internal/abstract/archive.h \
    # odr/internal/abstract/document.h \
    # odr/internal/abstract/document_cursor.h \
    # odr/internal/abstract/document_element.h \
    # odr/internal/abstract/document_index.h \
    # odr/internal/abstract/file.h \
    # odr/internal/abstract/filesystem.h \
    # odr/internal/cfb/cfb_archive.h \
    # odr/internal/cfb/cfb_impl.h \
    # odr/internal/cfb/cfb_util.h \
    # odr/internal/common/archive.h \
    # odr/internal/common/document_cursor.h \
    # odr/internal/common/document_element.h \
    # odr/internal/common/document_path.h \
    # odr/internal/common/file.h \
    # odr/internal/common/filesystem.h \
    # odr/internal/common/image_file.h \
    # odr/internal/common/path.h \
    # odr/internal/common/random.h \
    # odr/internal/common/style.h \
    # odr/internal/common/table_cursor.h \
    # odr/internal/common/table_position.h \
    # odr/internal/common/table_range.h \
    # odr/internal/common/temporary_file.h \
    # odr/internal/crypto/crypto_util.h \
    # odr/internal/git_info.h \
    # odr/internal/html/common.h \
    # odr/internal/html/css_writer.h \
    # odr/internal/html/document.h \
    # odr/internal/html/document_element.h \
    # odr/internal/html/document_style.h \
    # odr/internal/html/image_file.h \
    # odr/internal/html/text_file.h \
    # odr/internal/magic.h \
    # odr/internal/odf/odf_crypto.h \
    # odr/internal/odf/odf_cursor.h \
    # odr/internal/odf/odf_document.h \
    # odr/internal/odf/odf_element.h \
    # odr/internal/odf/odf_file.h \
    # odr/internal/odf/odf_manifest.h \
    # odr/internal/odf/odf_meta.h \
    # odr/internal/odf/odf_style.h \
    # odr/internal/oldms/oldms_file.h \
    # odr/internal/ooxml/ooxml_crypto.h \
    # odr/internal/ooxml/ooxml_file.h \
    # odr/internal/ooxml/ooxml_meta.h \
    # odr/internal/ooxml/ooxml_util.h \
    # odr/internal/ooxml/presentation/ooxml_presentation_cursor.h \
    # odr/internal/ooxml/presentation/ooxml_presentation_document.h \
    # odr/internal/ooxml/presentation/ooxml_presentation_element.h \
    # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_cursor.h \
    # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_document.h \
    # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_element.h \
    # odr/internal/ooxml/spreadsheet/ooxml_spreadsheet_style.h \
    # odr/internal/ooxml/text/ooxml_text_cursor.h \
    # odr/internal/ooxml/text/ooxml_text_document.h \
    # odr/internal/ooxml/text/ooxml_text_element.h \
    # odr/internal/ooxml/text/ooxml_text_style.h \
    # odr/internal/open_strategy.h \
    # odr/internal/project_info.h \
    # odr/internal/resource.h \
    # odr/internal/resource_data.h \
    # odr/internal/svm/svm_file.h \
    # odr/internal/svm/svm_format.h \
    # odr/internal/svm/svm_to_svg.h \
    # odr/internal/util/file_util.h \
    # odr/internal/util/hash_util.h \
    # odr/internal/util/map_util.h \
    # odr/internal/util/stream_util.h \
    # odr/internal/util/string_util.h \
    # odr/internal/util/xml_util.h \
    # odr/internal/zip/zip_archive.h \
    # odr/internal/zip/zip_util.h \
    # odr/open_document_reader.h \
    # odr/quantity.h \
    # odr/style.h \
    reportgenerator.h \
    tablemodel.h \
    tableproxymodel.h \
    ui/behaviortype.h \
    ui/boolfilterwidget.h \
    ui/collapsiblewidget.h \
    ui/columnsettingswidget.h \
    ui/columnspecificsettingswidget.h \
    ui/datetimefilterwidget.h \
    ui/filterswidget.h \
    ui/filtervalueedit.h \
    ui/filterwidget.h \
    ui/horizontalseparator.h \
    ui/intcolumnsettingswidget.h \
    ui/mainwindow.h \
    ui/numberfilterwidget.h \
    ui/reportsettingswidget.h \
    ui/stringcolumnsettingswidget.h \
    ui/stringfilterwidget.h \
    ui/stringvaluesfilterwidget.h \
    ui/tableheaderpreviewwidget.h \
    ui/tablesettingswidget.h \
    ui/tablewidget.h \
    ui/verticalseparator.h \
    utils.h

RESOURCES += \
    resources.qrc

DISTFILES += \
    odr/internal/cfb/README.md \
    odr/internal/odf/README.md
