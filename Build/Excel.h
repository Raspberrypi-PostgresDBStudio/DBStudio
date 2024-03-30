#ifndef EXCEL_H
#define EXCEL_H

#include <fstream>
#include <vector>

namespace excel
{
    enum XmlHorzAlign
    {
        haGeneral = 0,
        haLeft = 1,
        haRight = 2,
        haCentre = 3,
        haJustify = 4,
        haFill = 5,
        haCentreAcross = 6
    };
    enum XmlVerticalAlign
    {
        vaTop = 0,
        vaCentre = 1,
        vaBottom = 2
    };
    
    class XmlWriterCellStyle
    {
        private:
            std::string horzAlign[7] = { "General", "Left", "Right", "Center", "Justify", "Fill", "CentreAcross" };
            std::string vertAlign[3] = { "Top", "Center", "Bottom" };
            std::string styleName;
            std::string fontName;
            int fontSize;
            int fontColor;
            bool bold;
            bool italic;
            bool border;
            int cellColor;
            bool wordWrap;
            std::string formatString;
            XmlHorzAlign horizontalAlign;
            XmlVerticalAlign verticalAlign;
            
        public:
            XmlWriterCellStyle() {styleName = std::string();}
            XmlWriterCellStyle(std::string styleName, XmlWriterCellStyle style);
            virtual ~XmlWriterCellStyle(void);
            
            void StyleName(std::string style) {styleName = style;}
            std::string StyleName(void) {return styleName;}
            std::string FontName(void)  {return fontName;}
            void FontName(const std::string& val) { fontName = val;}
            int FontSize(void) {return fontSize;}
            void FontSize(int val) {fontSize = val;}
            int FontColor(void) {return fontColor;}
            void FontColor(int val) {fontColor = val;}
            bool Bold(void) {return bold;}
            void Bold(int val) {bold = val;} 
            bool Italic(void) {return italic;}
            void Italic(bool val) {italic = val;}
            bool Border(void) {return border;}
            void Border(bool val) {border = val;} 
            int CellColor(void) {return cellColor;}
            void CellColor(int val) {cellColor = val;}
            std::string FormatString(void) {return formatString;}
            void FormatString(std::string val) {formatString = val;}
            XmlHorzAlign HorizontalAlign(void) {return horizontalAlign;}
            void HorizontalAlign(XmlHorzAlign val) {horizontalAlign = val;}
            XmlVerticalAlign VerticalAlign(void) {return verticalAlign;}
            void VerticalAlign(XmlVerticalAlign val) { verticalAlign = val;}
            bool WordWrap(void) {return wordWrap;}
            void WordWrap(bool val) {wordWrap = val;}            
            void WriteStyleXml(std::ofstream& os);
    };
    
    class BaseStyle
    {
        protected:
            XmlWriterCellStyle excelStyle;
            
        public:
            BaseStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haLeft);
                excelStyle.VerticalAlign(XmlVerticalAlign::vaTop);
                excelStyle.FontSize(10);
                excelStyle.FontName("Arial");
                excelStyle.FontColor(0x000000);
                excelStyle.Bold(false);
                excelStyle.Italic(false);
                excelStyle.CellColor(0xFFFFFF);
                excelStyle.Border(false);
                excelStyle.WordWrap(false);
            }
            virtual ~BaseStyle(void) {}
            XmlWriterCellStyle getStyle() {return excelStyle;}
    };
    
    class WordWrapStyle : public BaseStyle
    {
        public:
            WordWrapStyle(void) {excelStyle.WordWrap(true);}
    };
    
    class HeaderStyle : public BaseStyle
    {
        public:
            HeaderStyle(void) { excelStyle.FontSize(excelStyle.FontSize() + 4); excelStyle.Bold(true); }
    };
    
    class SubHeaderCentreStyle : public BaseStyle
    {
        public:
            SubHeaderCentreStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haCentre);
                excelStyle.FontSize(excelStyle.FontSize() + 2);
                excelStyle.Bold(true);
            }
    };
    
    class SubHeaderStyle : public BaseStyle
    {
        public:
            SubHeaderStyle(void)
            {
                excelStyle.FontSize(excelStyle.FontSize() + 2);
                excelStyle.Bold(true);
            }
    };

    class TitleStyle : public BaseStyle
    {
        public:
            TitleStyle(void) {excelStyle.Bold(true);}
    };
    
    class TitleStyleRight : public BaseStyle
    {
        public:
            TitleStyleRight(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.Bold(true);
            }
    };
    
    class TotalStyle : public BaseStyle
    {
        public:
            TotalStyle(void)
            {
                excelStyle.Bold(true);
                excelStyle.Border(true);
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("Standard");
            }
    };

    class NumberStyle : public BaseStyle
    {
        public:
            NumberStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("Standard");
            }
    };
    
    class MoneyStyle : public BaseStyle
    {
        public:
            MoneyStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("#,##0.00");
            }
    };
    
    class IntegerStyle : public BaseStyle
    {
        public:
            IntegerStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("0");
            }
    };
    
    class DecimalStyle : public BaseStyle
    {
        public:
            DecimalStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("#,##0.0000");
            }
    };
    
    class PercentStyle : public BaseStyle
    {
        public:
            PercentStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("Percent");
            }
    };
    
    class DateStyle : public BaseStyle
    {
        public:
            DateStyle(void)
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("Short Date");
            }
    };
    
    class DateTimeStyle : public BaseStyle
    {
        public:
            DateTimeStyle()
            {
                excelStyle.HorizontalAlign(XmlHorzAlign::haRight);
                excelStyle.FormatString("dd/mm/yyyy hh:mm");
            }
    };
    
    /*
    pqxx::work txn(c);
    auto&& rst = txn.exec("SELECT date FROM table");

    for(auto&& row : rst)
      time_point date = row[0].as<time_point>();
    * */    
    class XmlWriterCell
    {
        std::string styleName;
        int index;
        std::string cellValue;
        std::string dataType;
        int mergeAcross = 0;
        std::string formula = std::string();

        public:
            XmlWriterCell(int ColIndex) {index = ColIndex;}
            virtual ~XmlWriterCell(void) { }
            std::string Value(void) {return cellValue;}
            void Value(std::string val, std::string style, std::string datatype);
            std::string StyleName(void) {return styleName;}
            void StyleName(std::string val) {styleName = val;}
            int MergeAcross(void) {return mergeAcross;}
            void MergeAcross(int val) {mergeAcross = val;}
            std::string Formula(void) {return formula;}
            void Formula(std::string val) {formula = val;}
            int ColumnIndex(void) {return index;}
            void WriteCellXml(std::ofstream& os);
    };
    
    class XmlWriterRow
    {
        private:
            std::vector<XmlWriterCell> workSheetCells;
            int index;
            
        public:
            XmlWriterRow(int rowIndex) {index = rowIndex;}
            virtual ~XmlWriterRow(void) { }
            int RowIndex(void) {return index;}
            XmlWriterCell *Cell(int col);
            void WriteRowXml(std::ofstream& os);
    };
    
    class XmlWriterColumn
    {
        private:
            int index;
            int span;
            double width;
            double ratio = 5.50;
            
        public:
            XmlWriterColumn(int colIndex, double colWidth) {index = colIndex; width = colWidth; ratio = 5.5; span = 0;}
            XmlWriterColumn(int colIndex, double colWidth, int colSpan) : XmlWriterColumn(colIndex, colWidth) {span = colSpan;}
            virtual ~XmlWriterColumn(void) { }
            int ColumnIndex(void) {return index;}
            void WriteColumnXml(std::ofstream& os);
    };
    
    class XmlWriterWorkSheet
    {
        private:
            std::vector<XmlWriterRow> workSheetRows;
            std::vector<XmlWriterColumn> workSheetColumns;
            std::string worksheetName;
            XmlWriterRow *getRow(int row);

        public:
            XmlWriterWorkSheet(std::string worksheetName) {this->worksheetName = worksheetName;}
            virtual ~XmlWriterWorkSheet(void) { }
            bool AddColumnDef(XmlWriterColumn col);
            XmlWriterCell *Cell(int row, int col) {return this->getRow(row)->Cell(col);}
            void WriteWorkSheetXml(std::ofstream& os);
    };
    
    class XmlWriter
    {
        private:
            std::vector<XmlWriterCellStyle> excelStyleTypes;
            std::vector<XmlWriterWorkSheet> workSheets;
            std::string filename;
            
        public:
            XmlWriter(std::string file) {this->filename = file;}
            virtual ~XmlWriter(void) { }
            std::string AddStyle(XmlWriterCellStyle style);
            XmlWriterWorkSheet AddSheet(std::string workSheet);
            XmlWriterWorkSheet AddSheet(unsigned int index, std::string workSheet);
            XmlWriterWorkSheet *Sheet(unsigned int index);
            std::string FileName(void) {return filename;}
            bool WriteToFile(void);
    };
}
#endif
