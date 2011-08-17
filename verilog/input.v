module mux_using_assign();
   trireg [0:15] #2.8 buf_out = en? in: 16'bz;
   assign mux_out = sel? a : b;
endmodule // End of module mux
