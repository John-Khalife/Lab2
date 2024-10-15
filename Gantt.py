import plotly.graph_objects as go
import pandas as pd

def drawGantt(data):
    # Create a DataFrame
    df = pd.DataFrame(data)

    # Create a figure
    fig = go.Figure()

    # Add filled rectangles for each event with different colors
    for index, row in df.iterrows():
        # Calculate the midpoint for label placement
        midpoint = (row['Start'] + row['End']) / 2
        
        # Add filled rectangle for the event
        fig.add_trace(go.Scatter(
            x=[row['Start'], row['End'], row['End'], row['Start'], row['Start']],  # Create a closed shape
            y=[0, 0, 1, 1, 0],  # y-coordinates for the rectangle (full height)
            fill='toself',      # Fill the area under the line
            line=dict(width=0), # No border line
            fillcolor=row['Color'],  # Fill color from the dataframe
            name=row['Event']
        ))

        # Add event name as text in the middle of the block
        fig.add_trace(go.Scatter(
            x=[midpoint],  # Use the midpoint for the x position
            y=[0.5],       # Set the y value to the middle (0.5)
            mode='text',
            text=row['Event'],
            textposition='middle center',  # Position text in the middle of the block
            showlegend=False,  # Hide the legend for the text traces
            textfont=dict(
                size=16,  # Set font size
                color='white'  # Set text color to white
            )
        ))

    # Update layout
    fig.update_layout(
        title='First Come First Serve',
        xaxis_title='Time (sec)',
        yaxis=dict(visible=False),  # Hide y-axis completely
        xaxis=dict(visible=False),  # Hide y-axis completely
        showlegend=False,
        title_x=0.5,
        title_y=0.95,  # Adjust this value to move the title closer to the graph
        paper_bgcolor='white',  # Set the outer background color to white
        plot_bgcolor='white',   # Set the plot area background color to white
    )

    # Add markers for the start and end points
    for start in df['Start']:
        fig.add_trace(go.Scatter(
            x=[start],  # Start point
            y=[0],      # Y position for marker
            mode='markers+text',  # Show marker and text
            marker=dict(size=10, color='black'),  # Marker style
            textfont=dict(
                size=10,  # Set font size
                color='black'  # Set text color to black
            ),
            text=[start],  # Marker label
            textposition='bottom center',  # Position label below marker
            showlegend=False  # Hide legend for this trace
        ))

    for end in df['End']:
        fig.add_trace(go.Scatter(
            x=[end],  # End point
            y=[0],    # Y position for marker
            mode='markers+text',  # Show marker and text
            marker=dict(size=10, color='black'),  # Marker style
            textfont=dict(
                size=10,  # Set font size
                color='black'  # Set text color to black
            ),
            text=[end],  # Marker label
            textposition='bottom center',  # Position label below marker
            showlegend=False  # Hide legend for this trace
        ))

    # Show the plot
    fig.show()
