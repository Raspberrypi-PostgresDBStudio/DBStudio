#include <sstream>
#include <iomanip>
#include <ctime>
#include <math.h>
#include "Excel.h"

namespace excel
{
    XmlWriterCellStyle::XmlWriterCellStyle(std::string stylename, XmlWriterCellStyle style)
    {
        this->styleName = stylename;
        this->bold = style.Bold();
        this->border = style.Border();
        this->cellColor = style.CellColor();
        this->fontColor = style.FontColor();
        this->fontName = style.FontName();
        this->fontSize = style.FontSize();
        this->formatString = style.FormatString();
        this->horizontalAlign = style.HorizontalAlign();
        this->italic = style.Italic();
        this->verticalAlign = style.VerticalAlign();
        this->wordWrap = style.WordWrap();
    }

    XmlWriterCellStyle::~XmlWriterCellStyle(void)
    {

    }

    void XmlWriterCellStyle::WriteStyleXml(std::ofstream& os)
    {
        std::stringstream ss;
        os << "<Style ss:ID=\"" << styleName << "\">\n";
        os << "<Alignment ss:Horizontal=\"" << horzAlign[(int)horizontalAlign] << "\" ss:Vertical=\"" << vertAlign[(int)verticalAlign] << "\""  << ((wordWrap) ? " ss:WrapText=\"1\""  : std::string()) << "/>\n";
        
        if(border)
            os << "<Borders>\n<Border ss:Position=\"Bottom\" ss:LineStyle=\"Continuous\" ss:Weight=\"2\"/>" << "<Border ss:Position=\"Top\" ss:LineStyle=\"Continuous\" ss:Weight=\"2\"/>\n</Borders>\n";
 
        if (bold)
            ss << " ss:Bold=\"1\"";
        if (italic)
            ss << " ss:Italic=\"1\"";
        ss << " ss:Size=\"" << fontSize << "\"";
        ss << " ss:FontName=\"" << fontName << "\"";
        ss << " ss:Color=\"#" << std::setfill('0') << std::setw(6) << std::hex << fontColor << "\"";
        os << "<Font" << ss.str() << "/>\n";
        if(cellColor != 0xFFFFFF)
            os << "<Interior ss:Color=\"#"<< std::setfill('0') << std::setw(6) << std::hex << cellColor << "\" ss:Pattern=\"Solid\"/>\n";

        if(!formatString.empty())
            os << "<NumberFormat ss:Format=\"" << formatString << "\"/>\n";

        os << "</Style>\n";
    }
    
    void XmlWriterCell::Value(std::string val, std::string style, std::string datatype)
    {
        this->cellValue = val;
        this->styleName = style;
        this->dataType = datatype;
    }

    void XmlWriterCell::WriteCellXml(std::ofstream& os)
    {
        os << "<Cell";
        if(mergeAcross != 0)
            os << "ss:MergeAcross=\"" << mergeAcross <<"\"";
        if(!styleName.empty())
            os << " ss:StyleID=\"" << styleName;
        os << "\" ss:Index=\"" << (index + 1) << "\"";
        if (!formula.empty())
            os << " ss:Formula=\"" << formula << "\"";
        os << ">";
                
        os << "<Data ss:Type=\"" << dataType << "\">" << cellValue << "</Data>";
        os << "</Cell>\n";
    }
   
    XmlWriterCell *XmlWriterRow::Cell(int col)
    {
        if (workSheetCells.size() > 0 && workSheetCells[workSheetCells.size() - 1].ColumnIndex() >= col)
        {
            for (unsigned int i = workSheetCells.size() - 1; i >= 0; i--)
            {
                if (workSheetCells[i].ColumnIndex() == col)
                    return &workSheetCells[i];
                if (workSheetCells[i].ColumnIndex() < col)
                {
                    //+1 might be an issue
                    workSheetCells.insert(workSheetCells.begin() + i + 1, XmlWriterCell(col));
                    return &workSheetCells[i + 1];
                }
            }
        }
        workSheetCells.push_back(XmlWriterCell(col));
        return &workSheetCells[workSheetCells.size() - 1];
    }
    
    void XmlWriterRow::WriteRowXml(std::ofstream& os)
    {
        os << "<Row ss:Index=\"" << (index + 1) << "\">\n";
        for (unsigned int i = 0; i < workSheetCells.size(); i++)
            workSheetCells[i].WriteCellXml(os);
        os << "</Row>\n";
    }
    
    void XmlWriterColumn::WriteColumnXml(std::ofstream& os)
    {
        std::stringstream ss;
        ss << "<Column ss:Index=\"" << (index + 1) << "\" ss:AutoFitWidth=\"0\" ss:Width=\"" << std::setprecision( 2 ) << lround(width * ratio) << "\"";
        if(span != 0)
            ss << " ss:Span=\"" << span << "\"";
        ss << "/>\n";
        
        os << ss.str();
    }
   
    bool XmlWriterWorkSheet::AddColumnDef(XmlWriterColumn col)
    {
        if (workSheetColumns.size() > 0 && workSheetColumns[workSheetColumns.size() - 1].ColumnIndex() >= col.ColumnIndex())
        {
            for (unsigned int i = workSheetColumns.size() - 1; i >= 0; i--)
            {
                if (workSheetColumns[i].ColumnIndex() == col.ColumnIndex())
                    return false;
                if (workSheetColumns[i].ColumnIndex() < col.ColumnIndex())
                {
                    workSheetColumns.insert(workSheetColumns.begin() + i + 1, col);
                    return true;
                }
            }
        }
        workSheetColumns.push_back(col);
        return true;
    }
    
    XmlWriterRow *XmlWriterWorkSheet::getRow(int row)
    {
        if (workSheetRows.size() > 0 && workSheetRows[workSheetRows.size() - 1].RowIndex() >= row)
        {
            for (unsigned int i = workSheetRows.size() - 1; i >= 0; i--)
            {
                if(workSheetRows[i].RowIndex() == row)
                    return &workSheetRows[i];
                if (workSheetRows[i].RowIndex() < row)
                {
                    workSheetRows.insert(workSheetRows.begin() + i + 1, XmlWriterRow(row));
                    return &workSheetRows[i + 1];
                }
            }
        }
        workSheetRows.push_back(XmlWriterRow(row));
        return &workSheetRows[workSheetRows.size() - 1];
    }
    
    void XmlWriterWorkSheet::WriteWorkSheetXml(std::ofstream& os)
    {
        os << "<Worksheet ss:Name=\"" << worksheetName << "\">\n<Table>\n";
        for(unsigned int i = 0 ; i < workSheetColumns.size() ; i++)
            workSheetColumns[i].WriteColumnXml(os);
        for (unsigned int i = 0; i < workSheetRows.size(); i++)
            workSheetRows[i].WriteRowXml(os);
        os << "</Table>\n</Worksheet>";
    }
    
    std::string XmlWriter::AddStyle(XmlWriterCellStyle style)
    {
        std::stringstream ss;
        ss << "s" << excelStyleTypes.size();
        style.StyleName(ss.str());
        excelStyleTypes.push_back(style);
        return ss.str();
    }
    
    XmlWriterWorkSheet XmlWriter::AddSheet(std::string workSheet)
    {
        workSheets.push_back(XmlWriterWorkSheet(workSheet));
        return workSheets[workSheets.size() - 1];
    }
    
    XmlWriterWorkSheet XmlWriter::AddSheet(unsigned int index, std::string workSheet)
    {
        workSheets.insert(workSheets.begin() + index, XmlWriterWorkSheet(workSheet));
        return workSheets[index];
    }
    
    XmlWriterWorkSheet *XmlWriter::Sheet(unsigned int index)
    {
        if( index < workSheets.size() )
            return &workSheets[index];
        return nullptr;
    }
    
    bool XmlWriter::WriteToFile()
    {
        std::ofstream os;
        os.open (filename, std::ios::trunc);
        os << "<?xml version=\"1.0\"?>\n<?mso-application progid=\"Excel.Sheet\"?>\n<Workbook xmlns=\"urn:schemas-microsoft-com:office:spreadsheet\" xmlns:o=\"urn:schemas-microsoft-com:office:office\" xmlns:x=\"urn:schemas-microsoft-com:office:excel\" xmlns:ss=\"urn:schemas-microsoft-com:office:spreadsheet\" xmlns:html=\"http://www.w3.org/TR/REC-html40\">\n";
        os << "<Styles>\n";
        for (unsigned int i = 0 ; i < excelStyleTypes.size() ; i++)
            excelStyleTypes[i].WriteStyleXml(os);
        os << "</Styles>\n";
        for (unsigned int i = 0 ; i < workSheets.size(); i++)
            workSheets[i].WriteWorkSheetXml(os);
        os << "</Workbook>";
        os.close();

        return true;
    }
}
