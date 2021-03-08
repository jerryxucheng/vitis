#include <hls_stream.h>
#include <string.h>
#include "graph_fpga.h"

#include "fpga_global_mem.h"
#include "fpga_slice.h"
#include "fpga_gather.h"
#include "fpga_filter.h"
#include "fpga_process_edge.h"
#include "fpga_cache.h"
#include "fpga_edge_prop.h"




extern "C" {
    void  readEdgesCU4(
        uint16          *edgesHeadArray,
        uint16          *vertexPushinProp,
        uint16          *edgesTailArray,
        uint16          *tmpVertexProp,
#if HAVE_EDGE_PROP
        uint16          *edgeProp,
#endif
        int             edge_end,
        int             sink_offset,
        int             sink_end
    )
    {
        const int stream_depth_filter = QUEUE_SIZE_FILTER;

        const int stream_depth_memory = QUEUE_SIZE_MEMORY;
#pragma HLS DATAFLOW



#pragma HLS INTERFACE m_axi port=edgesHeadArray offset=slave bundle=gmem0 max_read_burst_length=64
#pragma HLS INTERFACE s_axilite port=edgesHeadArray bundle=control

#pragma HLS INTERFACE m_axi port=edgesTailArray offset=slave bundle=gmem2 max_read_burst_length=64
#pragma HLS INTERFACE s_axilite port=edgesTailArray bundle=control

#pragma HLS INTERFACE m_axi port=tmpVertexProp offset=slave bundle=gmem1 max_read_burst_length=64 num_write_outstanding=4
#pragma HLS INTERFACE s_axilite port=tmpVertexProp bundle=control

#pragma HLS INTERFACE m_axi port=vertexPushinProp offset=slave bundle=gmem1 max_read_burst_length=64 num_write_outstanding=4
#pragma HLS INTERFACE s_axilite port=vertexPushinProp bundle=control

#if HAVE_EDGE_PROP

#pragma HLS INTERFACE m_axi port=edgeProp offset=slave bundle=gmem3 max_read_burst_length=64
#pragma HLS INTERFACE s_axilite port=edgeProp bundle=control

#endif
#pragma HLS INTERFACE s_axilite port=edge_end       bundle=control
#pragma HLS INTERFACE s_axilite port=sink_offset    bundle=control
#pragma HLS INTERFACE s_axilite port=sink_end       bundle=control
#pragma HLS INTERFACE s_axilite port=return         bundle=control



        hls::stream<int2_token>           buildArray[PE_NUM];
#pragma HLS stream variable=buildArray  depth=2

        hls::stream<int2_token>           buildArrayRes[PE_NUM];
#pragma HLS stream variable=buildArrayRes  depth=2

        hls::stream<int2_token>           buildArraySlice[PE_NUM];
#pragma HLS stream variable=buildArraySlice  depth=2

        hls::stream<edge_tuples_t>   edgeTuplesArray[PE_NUM];
#pragma HLS stream variable=edgeTuplesArray  depth=2

        hls::stream<filter_type>    toFilterArray[PE_NUM];
#pragma HLS stream variable=toFilterArray  depth=stream_depth_filter

        hls::stream<filter_type>    toFilterArraySlice[PE_NUM];
#pragma HLS stream variable=toFilterArraySlice  depth=4

        hls::stream<filter_type>    toFilterArraySlice2[PE_NUM];
#pragma HLS stream variable=toFilterArraySlice2  depth=4


        hls::stream<uint_uram>    writeArray[PE_NUM];
#pragma HLS stream variable=writeArray  depth=2

        hls::stream<uint_uram>    writeArrayLayer2[PE_NUM];
#pragma HLS stream variable=writeArrayLayer2  depth=2

        hls::stream<uint_uram>    writeArrayLayer1[PE_NUM];
#pragma HLS stream variable=writeArrayLayer1  depth=2


        hls::stream<burst_token>      edgeBurstSliceStream;
#pragma HLS stream variable=edgeBurstSliceStream depth=2
        hls::stream<burst_token>      edgeBurstStream;
#pragma HLS stream variable=edgeBurstStream depth=2
        hls::stream<burst_token>      edgeBurstStreamTmp;
#pragma HLS stream variable=edgeBurstStreamTmp depth=stream_depth_memory

        hls::stream<burst_token>      mapSliceStream;
#pragma HLS stream variable=mapSliceStream depth=2
        hls::stream<burst_token>      mapStream;
#pragma HLS stream variable=mapStream depth=2
        hls::stream<burst_token>      mapStreamTmp;
#pragma HLS stream variable=mapStreamTmp depth=stream_depth_memory

#if HAVE_EDGE_PROP
        hls::stream<burst_token>      edgePropSliceStream;
#pragma HLS stream variable=edgePropSliceStream depth=2
        hls::stream<burst_token>      edgePropStream;
#pragma HLS stream variable=edgePropStream depth=2
        hls::stream<burst_token>      edgePropStreamTmp;
#pragma HLS stream variable=edgePropStreamTmp depth=stream_depth_memory

#endif

        hls::stream<edge_tuples_t>   edgeTuplesCoupled;
#pragma HLS stream variable=edgeTuplesCoupled depth=2

        hls::stream<edge_tuples_t>   edgeTuplesBuffer;
#pragma HLS stream variable=edgeTuplesBuffer depth=2

        hls::stream<edge_tuples_t>   edgeTuplesLayer1[4];
#pragma HLS stream variable=edgeTuplesLayer1 depth=2


        edge_tuples_t tuples[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=tuples dim=0 complete

        filter_type filter[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=filter dim=0 complete

        uchar opcode[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=opcodep dim=0 complete

        shuffled_type shuff_ifo[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=shuff_ifo dim=0 complete


        uint_uram tmpVPropBuffer[PE_NUM][(MAX_VERTICES_IN_ONE_PARTITION / 2) >> (LOG2_PE_NUM)];
#pragma HLS ARRAY_PARTITION variable=tmpVPropBuffer dim=1 complete
#pragma HLS RESOURCE variable=tmpVPropBuffer core=XPM_MEMORY uram



        filter_type                 filter_tmp[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=filter_tmp dim=0 complete
        uint_raw                    filter_num[PE_NUM];
#pragma HLS ARRAY_PARTITION variable=filter_num dim=0 complete



        //printf("%d %d \n",(int)edge_offset,(int)edge_end );
        burstRead(0, edge_end, edgesTailArray, edgeBurstStream);
        sliceStream(edgeBurstStream, edgeBurstStreamTmp);
        sliceStream(edgeBurstStreamTmp, edgeBurstSliceStream);

        burstRead(0, edge_end, edgesHeadArray, mapStream);
        sliceStream(mapStream, mapStreamTmp);
        sliceStream(mapStreamTmp, mapSliceStream);

#if HAVE_EDGE_PROP
        sliceStream(edgePropStream, edgePropStreamTmp);
        sliceStream(edgePropStreamTmp, edgePropSliceStream);

#endif

        srcPropertyProcess(vertexPushinProp, edgeBurstSliceStream, mapSliceStream, edgeTuplesBuffer);

#if HAVE_EDGE_PROP
        propProcess(edgePropSliceStream, edgeTuplesBuffer, edgeTuplesCoupled);
#else
        propProcessSelf(edgeTuplesBuffer, edgeTuplesCoupled);
#endif
        /* timing */
        duplicateStream4(edgeTuplesCoupled, edgeTuplesLayer1[0], edgeTuplesLayer1[1], edgeTuplesLayer1[2], edgeTuplesLayer1[3]);

        for (int i = 0; i < 4 ; i++)
        {
#pragma HLS UNROLL
            duplicateStream4(edgeTuplesLayer1[i],
                             edgeTuplesArray[i * 4 + 0],
                             edgeTuplesArray[i * 4 + 1],
                             edgeTuplesArray[i * 4 + 2],
                             edgeTuplesArray[i * 4 + 3] );
        }

#if 1
        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            shuffleEntry (
                i,
                edgeTuplesArray[i],
                toFilterArraySlice[i],
                filter[i],
                tuples[i],
                opcode[i],
                shuff_ifo[i]
            );
        }


        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            filterSlice(toFilterArraySlice[i], toFilterArray[i]);
        }
        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            filterSlice(toFilterArray[i], toFilterArraySlice2[i]);
        }
//    for (int i = 0; i < PE_NUM ; i++)
//    {
//#pragma HLS UNROLL
//        filterSlice(toFilterArraySlice3[i], toFilterArraySlice[i]);
//    }

        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            tupleFilter( filter_tmp[i], filter_num[i], toFilterArraySlice2[i], buildArray[i]);
        }


        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            processEdgesBuildSlice(buildArray[i], buildArraySlice[i]);
        }
        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            rawSolver(buildArraySlice[i], buildArrayRes[i]);
        }
        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            dstPropertyProcess(i, sink_offset, sink_end, tmpVPropBuffer[i], buildArrayRes[i], writeArrayLayer1[i]);
        }
        for (int i = 0; i < PE_NUM ; i++)
        {
#pragma HLS UNROLL
            processEdgesSlice(writeArrayLayer1[i], writeArray[i]);
        }

//    for (int i = 0; i < PE_NUM ; i++)
//    {
//#pragma HLS UNROLL
//        processEdgesSlice(writeArrayLayer2[i], writeArray[i]);
//    }

        processEdgeWrite(sink_offset, sink_end, writeArray, tmpVertexProp);

#endif


    }

} // extern C

