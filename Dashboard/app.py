import streamlit as st
import pandas as pd
import altair as alt
import plotly.express as px
import matplotlib.pyplot as plt
import matplotlib.patches as patches


import numpy as np
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import LabelEncoder, StandardScaler
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense
from tensorflow.keras.utils import to_categorical

st.set_page_config(
    page_title="TerraProbe SoilIQ",
    page_icon="",
    layout="wide",
    initial_sidebar_state="expanded"
)

alt.themes.enable("dark")

# Function to load CSS for styling
def load_css():
    custom_css = """
    .logo-container {
        display: flex;
        align-items: center;
        justify-content: space-between;
        width: 100%;
        padding: 10px;
    }
    
    .logo-text {
        font-size: 24px;  # Adjust font size as needed
        font-weight: bold;
        color: white;
        font-family: 'Arial', sans-serif;  # Use a nice, readable font
        flex-grow: 1;  # Make sure the text fills the remaining space
        text-align: right;  # Align text to the right
        padding-left: 20px;  # Space between logo and text
    }
    """
    st.markdown(f"<style>{custom_css}</style>", unsafe_allow_html=True)

# ✅ Call function AFTER set_page_config()
load_css()

def get_data():
    # Load the file
    with open('TEST.txt', 'r') as file:
        lines = file.readlines()

    # Clean each line: strip leading/trailing spaces and pipes, then split
    data = [
        [item.strip() for item in line.strip().strip('|').split('|')]
        for line in lines if line.strip()  # skip empty lines
    ]

    # Convert to DataFrame
    df = pd.DataFrame(data)

    # Optional: Assign column names
    df.columns = ['Data Point Number', 'Moisture (%)', 'Nitrogen (N)', 'Phosphorous (P)', 'Potassium (K)', 'Depth']
    return df

depth_profile = get_data()
# Sidebar content
with st.sidebar:
    logo_path = "TerraProbe_logo.png" 
    
    st.image(logo_path, use_container_width=True, width=100) 

with st.sidebar:
    st.title('TerraProbe Soil IQ')
    
    depth_list = list(depth_profile["Depth"].unique())[::-1]
    
    selected_depth = st.selectbox('Select a depth', depth_list, index=len(depth_list)-1)
    df_selected_depth = depth_profile[depth_profile["Depth"] == selected_depth]

numeric_cols = ["Moisture (%)", "Nitrogen (N)", "Phosphorous (P)", "Potassium (K)"]
df_selected_depth[numeric_cols] = df_selected_depth[numeric_cols].astype(float)

# Compute averages
if not df_selected_depth.empty:
    nitrogen = df_selected_depth["Nitrogen (N)"].mean()
    phosphorous = df_selected_depth["Phosphorous (P)"].mean()
    potassium = df_selected_depth["Potassium (K)"].mean()
    moisture = df_selected_depth["Moisture (%)"].mean()
else:
    nitrogen, phosphorous, potassium, moisture = 0, 0, 0, 0

st.markdown(f'### 🌱 Soil Depth Properties at Depth of **{selected_depth}** " ')

# Create 4 metric columns
col1, col2, col3, col4 = st.columns(4)

with col1:
    st.metric(label="Nitrogen (N)", value=f"{nitrogen}")

with col2:
    st.metric(label="Phosphorous (P)", value=f"{phosphorous}")

with col3:
    st.metric(label="Potassium (K)", value=f"{potassium}")

with col4:
    st.metric(label="Moisture", value=f"{moisture}")

st.markdown("### Depth Profile Data for Selected Depth")
st.dataframe(df_selected_depth) 

# Function to plot cylinder depth visualization

numeric_depth = int(selected_depth.split()[0])

def plot_cylinder(selected_depth):
    fig, ax = plt.subplots(figsize=(2, 5)) 
    ax.add_patch(plt.Rectangle((0.3, 0), 0.4, 12, edgecolor="black", facecolor="lightgray", lw=2, zorder=2))
    ax.add_patch(plt.Rectangle((0.3, 0), 0.4, 12, edgecolor="none", facecolor="gray", alpha=0.2, zorder=1))
    

    # Mark depth lines (start from the top)
    depth_marks = [2, 4, 8, 12]
    for depth in depth_marks:
        ax.hlines(y=12 - depth, xmin=0.2, xmax=0.8, color="black", linewidth=1)  # Invert depth
        ax.text(0.85, 12 - depth, f"{depth}\"", verticalalignment="center", fontsize=10, color="white")

    ax.annotate("➡", xy=(0.1, 12 - numeric_depth), xytext=(-0.2, 12 - selected_depth),
                fontsize=14, ha="center", va="center", color="red", zorder=3)

    ax.set_xlim(-0.5, 1)
    ax.set_ylim(0, 12)
    ax.axis("off")  # Hide axis

    fig.patch.set_facecolor("black")
    
    return fig

with st.sidebar:
    st.pyplot(plot_cylinder(numeric_depth))
    

depth_profile_avg = depth_profile.copy()
depth_profile_avg["Depth"] = depth_profile_avg["Depth"].str.extract(r"(\d+)").astype(int)

for col in ['Nitrogen (N)', 'Phosphorous (P)', 'Potassium (K)', 'Moisture (%)']:
    depth_profile_avg[col] = pd.to_numeric(depth_profile_avg[col], errors='coerce')

depth_profile_avg = depth_profile_avg.groupby("Depth", as_index=False).mean(numeric_only=True)

def plot_line_chart(data, y_column, title, color):
    chart = (
        alt.Chart(data)
        .mark_line(point=True)
        .encode(
            x=alt.X("Depth", title="Depth (inches)", sort="ascending"),
            y=alt.Y(y_column, title=title),
            color=alt.value(color),
            tooltip=["Depth", y_column]
        )
        .properties(width=250, height=250)
    )
    return chart

st.markdown("### 📈 Trends Across Depths")
plot_col1, plot_col2, plot_col3, plot_col4 = st.columns(4)

with plot_col1:
    st.altair_chart(plot_line_chart(depth_profile_avg, "Nitrogen (N)", "Nitrogen Levels", "#1f77b4"), use_container_width=True)

with plot_col2:
    st.altair_chart(plot_line_chart(depth_profile_avg, "Phosphorous (P)", "Phosphorous Levels", "#ff7f0e"), use_container_width=True)

with plot_col3:
    st.altair_chart(plot_line_chart(depth_profile_avg, "Potassium (K)", "Potassium Levels", "#2ca02c"), use_container_width=True)

with plot_col4:
    st.altair_chart(plot_line_chart(depth_profile_avg, "Moisture (%)", "Moisture Levels", "#d62728"), use_container_width=True)
    
# Load the crop recommendation dataset
crop_data = pd.read_csv("Crop_recommendation.csv")

# Feature columns for the neural network
features = ['N', 'P', 'K', 'temperature', 'humidity']
X = crop_data[features]

le = LabelEncoder()
y_encoded = le.fit_transform(crop_data['label'])
y_categorical = to_categorical(y_encoded)

# Normalize the feature inputs
scaler = StandardScaler()
X_scaled = scaler.fit_transform(X)

X_train, X_test, y_train, y_test = train_test_split(X_scaled, y_categorical, test_size=0.2, random_state=42)

model = Sequential([
    Dense(128, input_dim=X_train.shape[1], activation='relu'),
    Dense(64, activation='relu'),
    Dense(32, activation='relu'),
    Dense(y_categorical.shape[1], activation='softmax')  # Softmax for multi-class classification
])

model.compile(loss='categorical_crossentropy', optimizer='adam', metrics=['accuracy'])

history = model.fit(X_train, y_train, epochs=50, batch_size=16, validation_split=0.1, verbose=1)

loss, accuracy = model.evaluate(X_test, y_test, verbose=0)

st.title("Crop Recommendation Prediction")

temperature = st.number_input("Enter Temperature (°C)", min_value=-10, max_value=50, value=25)
humidity = st.number_input("Enter Humidity (%)", min_value=0, max_value=100, value=60)

input_data = np.array([[nitrogen, phosphorous, potassium, temperature, humidity]])

input_scaled = scaler.transform(input_data)

# Make a prediction
if st.button("Predict Crop"):
    prediction = model.predict(input_scaled)
    predicted_label = le.inverse_transform([np.argmax(prediction)])
    st.markdown(f"<h1 style='text-align: center;'>🌾 Recommended Crop: {predicted_label[0]}</h1>", unsafe_allow_html=True)