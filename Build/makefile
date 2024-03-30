CXX = g++
PROGRAM = DbStudio
CXXFILES = main.cc DbStudio.cc DbLogin.cc DbServer.cc DbServerList.cc Utility.cc DbTreeview.cc DbTextBuffer.cc DbUndoRedoList.cc Sql.cc DbMessage.cc Excel.cc
OBJS = $(CXXFILES:.cc=.o)
DEPDIR = .deps
CXXFLAGS = `pkg-config --cflags gtkmm-3.0` -std=c++17 -Wall -g -Wno-psabi
LIBS = -lpqxx -lpq `pkg-config --libs gtkmm-3.0`

all: $(PROGRAM)

-include $(OBJS:%.o=$(DEPDIR)/%.Po)

%.o: %.cc
	@mkdir -p $(DEPDIR)
	$(CXX) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $(CXXFLAGS) $<
	@mv -f $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po

$(PROGRAM): $(OBJS)
	$(CXX) -o $(PROGRAM) $(OBJS) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(PROGRAM)
