/*
* Copyright (c) 2019, NVIDIA CORPORATION.  All rights reserved.
*
* NVIDIA CORPORATION and its licensors retain all intellectual property
* and proprietary rights in and to this software, related documentation
* and any modifications thereto.  Any use, reproduction, disclosure or
* distribution of this software and related documentation without an express
* license agreement from NVIDIA CORPORATION is strictly prohibited.
*/

#pragma once

#include <stdint.h>

#include <stdio.h>

#include <claraparabricks/genomeworks/cudapoa/batch.hpp>

namespace claraparabricks
{

namespace genomeworks
{

namespace cudapoa
{

static bool use32bitScore(const BatchSize& batch_size, const int16_t gap_score, const int16_t mismatch_score, const int16_t match_score)
{
    // theoretical max score takes place when sequence and graph completely match with each other
    int32_t upper_bound = batch_size.max_sequence_size * match_score;
    // theoretical min score takes place when sequence and graph do not include a single match
    // it is assumed max_sequence_size <= max_matrix_graph_dimension; gap_score and match_scores are negative, and match_score is positive
    int32_t lower_bound = batch_size.max_sequence_size * std::max(gap_score, mismatch_score) + (batch_size.max_matrix_graph_dimension - batch_size.max_sequence_size) * gap_score;
    // if theoretical upper or lower bound exceed the range represented by int16_t, then int32_t should be used
    return (upper_bound > INT16_MAX || (-lower_bound) > (INT16_MAX + 1));
}

static bool use32bitSize(const BatchSize& batch_size, bool banded)
{
    int32_t max_length = batch_size.max_consensus_size;
    if (banded)
    {
        max_length = std::max(max_length, batch_size.max_matrix_graph_dimension_banded);
    }
    else
    {
        max_length = std::max(max_length, batch_size.max_matrix_graph_dimension);
    }
    max_length = std::max(max_length, batch_size.max_matrix_sequence_dimension);
    //if max array length in POA analysis exceeds the range represented by int16_t, then int32_t should be used
    return (max_length > INT16_MAX);
}

} // namespace cudapoa
} // namespace genomeworks
} // namespace claraparabricks
